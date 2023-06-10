#include "Client.h"

void Client::run() {
	window.Start();
	
	while (true) {
		window.Refresh();
		window.PollInputs();
	}

}