#include "Client/Profiler/PerformanceProfiler.h"
#include "Level/Timer/Timer.h"
#include "FileManager/Files.h"

PerformanceProfiler::PerformanceProfiler() {
    initial_time_ = std::chrono::high_resolution_clock::now();
}

void PerformanceProfiler::ProfileStart(uint64_t hash) {
    timer_stack_.emplace(hash, 0);
    timer_stack_.top().second = (std::chrono::high_resolution_clock::now() - initial_time_).count();
}

void PerformanceProfiler::ProfileStart(std::string path) {
    uint64_t hash = Hasher(path);
    if (!string_hash_container_.count(hash)) {
        RegisterPaths(path);
    }

    timer_stack_.emplace(hash, (std::chrono::high_resolution_clock::now() - initial_time_).count());
}

void PerformanceProfiler::ProfileStop(uint64_t hash) {
    uint64_t timePass = (std::chrono::high_resolution_clock::now() - initial_time_).count() - timer_stack_.top().second;
    time_pass_cache_.emplace_back(hash, timePass);
    timer_stack_.pop();
}

void PerformanceProfiler::ProfileStop(std::string path) {
    uint64_t hash = Hasher(path);
    uint64_t timePass = (std::chrono::high_resolution_clock::now() - initial_time_).count() - timer_stack_.top().second;

    time_pass_cache_.emplace_back(hash, timePass);
    timer_stack_.pop();
}

// TOOD: Fix me
void PerformanceProfiler::RegisterPaths(std::string path) {
    string_hash_container_[Hasher(path)] = path;
    std::vector<std::string> tokens = Tokenize(path, '/');
    std::vector<std::string> pathTokens = {};

    if (tokens.size() != 1) {
        pathTokens.insert(pathTokens.end(), tokens.begin(), tokens.end());
    }

    string_tokenized_hash_container_[Hasher(path)] = pathTokens;

}

uint64_t PerformanceProfiler::Hasher(std::string path) {
    return std::hash<std::string>{}(path);
}

void PerformanceProfiler::LoadCache() {
    CondenseCache();

    for (const auto& [hash, time] : time_pass_cache_) {
        std::vector<std::string> tokens = string_tokenized_hash_container_[hash];
        root_.ChangeTime(tokens, 0, (double)time);
    }

    time_pass_cache_.clear();
}

void PerformanceProfiler::Print() {
    root_.print();
}

void PerformanceProfiler::CondenseCache() {
    FastHashMap<uint64_t, uint64_t> condensedCache = {};

    for (const auto& [hash, time] : time_pass_cache_) {
        if (condensedCache.count(hash)) {
            condensedCache[hash] += time;
        }
        else {
            condensedCache[hash] = time;
        }
    }

    time_pass_cache_.clear();
    for (const auto& [hash, time] : condensedCache) {
        time_pass_cache_.emplace_back(hash, time);
    }

}

void PerformanceProfiler::CombineCache(PerformanceProfiler profiler) {
    //Simplify cache first
    profiler.CondenseCache();
    time_pass_cache_.insert(time_pass_cache_.end(), profiler.time_pass_cache_.begin(), profiler.time_pass_cache_.end());
    string_hash_container_.insert(profiler.string_hash_container_.begin(), profiler.string_hash_container_.end());
    string_tokenized_hash_container_.insert(profiler.string_tokenized_hash_container_.begin(), profiler.string_tokenized_hash_container_.end());
    CondenseCache();
}

PerformanceProfiler::PerformanceTree::PerformanceTree() = default;
PerformanceProfiler::PerformanceTree::PerformanceTree(std::string name) : name_(name) {};

void PerformanceProfiler::PerformanceTree::ChangeTime(std::vector<std::string>& path, int depth, double time) {
    time_passed_ += time;

    if (path.size() == depth) {
        return;
    }

    for (auto& node : nodes_) {
        if (!strcmp(node->name_.c_str(), path[depth].c_str())) {
            node->ChangeTime(path, depth + 1, time);
            return;
        }
    }

    nodes_.push_back(std::make_unique<PerformanceTree>(path[depth]));
    nodes_.back()->ChangeTime(path, depth + 1, time);
}

void PerformanceProfiler::PerformanceTree::print(int depth) const {
    std::string out = "";

    for (int i = 0; i < depth; i++) {
        out += ' ';
    }

    if (depth != 0) {
        out += '-';
    }

    out += name_ + ':';
    out += std::to_string(time_passed_ / 1000000.0) + " ms\n";
    std::cout << out;

    for (const auto& node : nodes_) {
        node->print(depth + 1);
    }
}