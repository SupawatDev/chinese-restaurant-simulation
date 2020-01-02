#include "simulator.h"

#include "event.h"

#include "variables.h"
#include "generator.h"

#include "customer_group.h"
#include "manager.h"
#include "waiter.h"
#include "cashier.h"
#include "table.h"
#include "seat.h"

#include "process.h"

Simulator::Simulator (const unsigned end_time, const Variables variables):	current_time_(0),
																			end_time_(end_time),
																			is_step_(false)
																	{
	// Initialize variables in simulator
	chinese_restaurant_                   = new ChineseRestaurant();
	chinese_restaurant_->variables        = new Variables(variables);
	chinese_restaurant_->random_generator = new Generator();
	// Initialize components in process
	process_ = new Process();
}

Simulator::~Simulator ( ) {
	delete chinese_restaurant_;
}

void Simulator::Init (bool is_step, Log::LogPriority level) {
	is_step_ = is_step;
	Log::GetLog()->SetPriority(static_cast<Log::LogPriority>(level));
	PrepareRestaurant();
}

void Simulator::PrepareRestaurant() const {
	// Copy Variables.
	auto var = chinese_restaurant_->variables;
	// Prepare the manager.
	chinese_restaurant_->manager = new Manager(chinese_restaurant_);
	// Prepare restaurant tables (2 3 4 persons).
	for (unsigned int i = 0; i < 3;++i) {
		for(unsigned int j = 0; j < var->number_tables[i]; ++j) {
			auto table = new Table(i + 2);
			chinese_restaurant_->free_restaurant_tables.push_back(table);
		}
	}
	// Prepare buffet seats.
	for (unsigned int i = 0; i < var->number_buffet_seats; i++) {
		auto seat = new Seat();
		chinese_restaurant_->free_buffet_seats.push_back(seat);
	}
	// Prepare waiters.
	for (unsigned int i = 0; i < var->number_waiters; ++i) {
		auto waiter = new Waiter();
		chinese_restaurant_->free_waiter_queue.push(waiter);
	}
	// Prepare cashier.
	for (unsigned int i = 0; i < var->number_cashiers; ++i) {
		auto cashier = new Cashier();
		chinese_restaurant_->free_cashiers.push(cashier);
	}
}

void Simulator::Run() {
	// Creating the first customer group.
	(new CustomerGroup(chinese_restaurant_, process_))->Activate(current_time_);
	// Run the simulation by popping the first event.
	while (current_time_ <= end_time_) {
		Event * event = process_->PopEvent();
		current_time_ = event->event_time;
		printf("-----------------------------------------------------------------------\n");
		printf("TIME: %d\n", current_time_);
		CustomerGroup * customer_group = event->customer_group;
		customer_group->Execute(current_time_);
		delete event;
		if (customer_group->IsTerminated()) delete customer_group;
		Status();
		if(is_step_) {
			Log::GetLog()->Print("Press any key to continue", Log::P1);
			system("pause");
		}
	}
}

void Simulator::Status() const {
	printf("````````STATUS```````````");


	printf("\n -- Front of Restaurant --");
	// Queue of Restaurant Customer Group
	printf("\nRestaurant Queue (%d groups): ", static_cast<int>(chinese_restaurant_->restaurant_queue.size()));
	if (!chinese_restaurant_->restaurant_queue.empty()) {
		for(auto & customer_group: chinese_restaurant_->restaurant_queue) {
			printf("#%d ", customer_group->GetCustomerGroupID());
		}
	}
	// Queue of Buffet Customer Group
	printf("\nBuffet Queue(%d groups): ", static_cast<int>(chinese_restaurant_->buffet_queue.size()));
	if(!chinese_restaurant_->buffet_queue.empty()) {
		auto temp_buffet_queue = chinese_restaurant_->buffet_queue;
		while (!temp_buffet_queue.empty()) {
			printf("#%d ", temp_buffet_queue.front()->GetCustomerGroupID());
			temp_buffet_queue.pop();
		}
	}

	
	printf("\n -- Inside of Restaurant -- ");
	// Available Waiters in the queue
	printf("\nFree Waiters Queue (%d persons): ", static_cast<int>(chinese_restaurant_->free_waiter_queue.size()));
	if(!chinese_restaurant_->free_waiter_queue.empty()) {
		auto temp_free_waiter_queue = chinese_restaurant_->free_waiter_queue;
		while(!temp_free_waiter_queue.empty()) {
			printf("#%d ", temp_free_waiter_queue.front()->GetWaiterID());
			temp_free_waiter_queue.pop();
		}	
	}
	
	// Available Tables in the restaurant
	printf("\nFree Table (%d tables): ", static_cast<int>(chinese_restaurant_->free_restaurant_tables.size()));
	for (auto & free_restaurant_table : chinese_restaurant_->free_restaurant_tables) printf("#%d(%d seats) ",
	                                                                                        free_restaurant_table->GetTableID(),
	                                                                                        free_restaurant_table->GetSeatNumber());
	// Available Buffet Seats in the restaurant
	printf("\nFree Buffet Seats (%d tables): ", static_cast<int>(chinese_restaurant_->free_buffet_seats.size()));
	for (auto & free_buffet_seat : chinese_restaurant_->free_buffet_seats) printf("#%d ", free_buffet_seat->GetSeatId());

	
	printf("\n -- Checkout point of Restaurant --");
	// Queue of Checkout Customer Group
	printf("\nCheckout Queue (%d groups): ", static_cast<int>(chinese_restaurant_->check_out_queue.size()));
	if (!chinese_restaurant_->check_out_queue.empty()) {
		auto temp_check_out_queue = chinese_restaurant_->check_out_queue;
		while (!temp_check_out_queue.empty()) {
			printf("#%d ", temp_check_out_queue.front()->GetCustomerGroupID());
			temp_check_out_queue.pop();
		}
	}
	// Available Cashiers
	printf("\nFree Cashier Queue (%d persons): ", static_cast<int>(chinese_restaurant_->free_cashiers.size()));
	if (!chinese_restaurant_->free_cashiers.empty()) {
		auto temp_free_cashier = chinese_restaurant_->free_cashiers;
		while (!temp_free_cashier.empty()) {
			printf("#%d ", temp_free_cashier.front()->GetCashierID());
			temp_free_cashier.pop();
		}
	}
	printf("\n");
}
