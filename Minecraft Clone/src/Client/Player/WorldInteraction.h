#pragma once
#include "../../Level/Typenames.h"

class Player;
class UserInputs;
class Dimension;

struct Ray;

class WorldInteraction {
public:
    void Interact(Player* player, const UserInputs& Inputs, Dimension* dimension);
private:
    BlockID GetBlock(Ray ray, Dimension* dimension);
    void BreakBlock(Ray ray, Dimension* dimension);
    void PlaceBlock(Ray ray, BlockID block, Dimension* dimension);
};