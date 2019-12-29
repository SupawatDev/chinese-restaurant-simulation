#ifndef MODEL_MANAGER_H
#define MODEL_MANAGER_H
#include <queue>
#include "table.h"

class CustomerGroup;
struct ChineseRestaurant;

class Manager {
public:
	Manager(ChineseRestaurant * chinese_restaurant);
	void Manages (CustomerGroup * this_customer, unsigned int current_time);
	~Manager();
	// if manages return true means the manager finally manages the queue;
private:
	ChineseRestaurant * chinese_restaurant_;
};


#endif // !MANAGER_MODEL_H