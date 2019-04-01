#include <iostream>

#include <cstdint>

#include <map>

#include <string>

#include <fstream>

#include <sstream>

struct Data {
	bool dirty = false;
	uint64_t tag;
	char pred = '@';
	uint64_t error = 0;
	unsigned int hist = (1 << 1) || (1 << 0);
	unsigned int histPredError [(1 << 1) || (1 << 0)];
};

int main(void) {

	std::map<std::string, std::string> branches;

	std::ifstream file("tokenizer.tra");

	std::string from;
	std::string result;
	std::string to;

	uint64_t total = 0;
	uint64_t correct = 0;

	/* LOAD FILE AND FIXED */

	while(file >> from >> result >> to) {
		from.erase(from.begin(), from.begin() + 2);

		if(branches.find(from) == branches.end()) {
			branches.insert(std::pair<std::string, std::string>(from, result));
		} else {
			branches.at(from) += result;
		}

		if(result[0] == '@') {
			++correct;
		}

		++total;
	}

	file.close();

	std::cout << "Fixed : " << correct << " / " << total << std::endl;
	correct = 0;

	/* STATIC FIRST */

	Data slots[1024];

	for(auto branch = branches.begin(); branch != branches.end(); ++branch) {

		std::stringstream ss;
		ss << std::hex << branch->first;
		uint64_t address;
		ss >> address;
		uint64_t top = address >> 10;
		uint64_t bottom = address & 0b1111111111;
	
		if(slots[bottom].tag != top) {
			slots[bottom].tag = top;
			slots[bottom].pred = branch->second[0];
		}

		char pred = slots[bottom].pred;

		for(unsigned int index = 0; index < branch->second.size(); ++index) {
			if(branch->second[index] == pred) {
				++correct;
			}
		}
	}

	std::cout << "Static : " << correct << " / " << total << std::endl;
	correct = 0;

	/* BIMODAL */
	for(uint64_t i = 0; i < 1024; ++i) {
		slots[i] = Data();
	}
	
	for(auto branch = branches.begin(); branch != branches.end(); ++branch) {

		std::stringstream ss;
		ss << std::hex << branch->first;
		uint64_t address;
		ss >> address;
		uint64_t top = address >> 10;
		uint64_t bottom = address & 0b1111111111;
	
		if(slots[bottom].tag != top) {
			slots[bottom].tag = top;
			slots[bottom].pred = '@';
			slots[bottom].error = 1;
		}

		char pred = slots[bottom].pred;

		for(unsigned int index = 0; index < branch->second.size(); ++index) {
			if(branch->second[index] == pred) {
				++correct;
				slots[bottom].error = 0;
			} else {
				slots[bottom].error += 1;
				if(slots[bottom].error == 2) {
					if(pred == '@') {
						pred = '.';
					} else {
						pred = '@';
					}
				}
			}
		}
	}

	std::cout << "Bimodal : " << correct << " / " << total << std::endl;
	correct = 0;

	/* 2-LAYER */
	for(uint64_t i = 0; i < 1024; ++i) {
		slots[i] = Data();
	}

	for(auto branch = branches.begin(); branch != branches.end(); ++branch) {

		std::stringstream ss;
		ss << std::hex << branch->first;
		uint64_t address;
		ss >> address;
		uint64_t top = address >> 10;
		uint64_t bottom = address & 0b1111111111;
	
		if(slots[bottom].tag != top) {
			slots[bottom].tag = top;
			slots[bottom].pred = '@';
			slots[bottom].error = 1;

			slots[bottom].hist = (1 << 1) | (1 << 1);

			slots[bottom].histPredError[0] = (0 << 1) | (1 << 0);
			slots[bottom].histPredError[1] = (1 << 1) | (1 << 0);
			slots[bottom].histPredError[2] = (0 << 1) | (1 << 0);
			slots[bottom].histPredError[3] = (1 << 1) | (1 << 0);
		}

		char pred = slots[bottom].pred;

		for(unsigned int index = 0; index < branch->second.size(); ++index) {
			if(branch->second[index] == pred) {
				++correct;
				slots[bottom].hist &= (1 << 0);
			} else {
				// if already has one error
				if(slots[bottom].histPredError[slots[bottom].hist] & (1 << 0)) {
					// flip the prediction and remove error
					slots[bottom].histPredError[slots[bottom].hist] ^= (1 << 1) | (1 << 0);


				}

					
				if(slots[bottom].histPredError[slots[bottom].hist] & (1 << 1) | (0 << 0)) {
					pred = '@';
				} else {
					pred = '.';
				}
			}
		}
	}

	std::cout << "2-Layer : " << correct << " / " << total << std::endl;

	return 0;
}
