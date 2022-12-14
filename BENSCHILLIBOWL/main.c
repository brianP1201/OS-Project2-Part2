#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "BENSCHILLIBOWL.h"

// Feel free to play with these numbers! This is a great way to
// test your implementation.
#define BENSCHILLIBOWL_SIZE 50
#define NUM_CUSTOMERS 40
#define NUM_COOKS 5
#define ORDERS_PER_CUSTOMER 3
#define EXPECTED_NUM_ORDERS NUM_CUSTOMERS * ORDERS_PER_CUSTOMER

// Global variable for the restaurant.
BENSCHILLIBOWL *bcb;

/**
 * Thread funtion that represents a customer. A customer should:
 *  - allocate space (memory) for an order.
 *  - select a menu item.
 *  - populate the order with their menu item and their customer ID.
 *  - add their order to the restaurant.
 */
void* BENSCHILLIBOWLCustomer(void* tid) {
    int customer_id = (int)(long) tid;
    int i;
		for(i=0; i<ORDERS_PER_CUSTOMER; i++) {
			Order* order = (Order*) malloc(sizeof(Order));
			order->menu_item = PickRandomMenuItem();
			order->customer_id = customer_id;
			order->next = NULL;
			AddOrder(bcb, order);
		}
}

/**
 * Thread function that represents a cook in the restaurant. A cook should:
 *  - get an order from the restaurant.
 *  - if the order is valid, it should fulfill the order, and then
 *    free the space taken by the order.
 * The cook should take orders from the restaurants until it does not
 * receive an order.
 */
void* BENSCHILLIBOWLCook(void* tid) {
    int cook_id = (int)(long) tid;
	int orders_fulfilled = 0;
	while (bcb->orders_handled != bcb-> expected_num_orders){
		Order* order = GetOrder(bcb);
		if (order){
			orders_fulfilled++;
			printf("Cook #%d fulfilled Customer %d's order. %d Orders left to be fulfilled.\n", cook_id, order->customer_id, (bcb->expected_num_orders-orders_fulfilled));
			free(order);
		}
	}
	printf("Cook #%d fulfilled %d orders\n", cook_id, orders_fulfilled);
}

/**
 * Runs when the program begins executing. This program should:
 *  - open the restaurant
 *  - create customers and cooks
 *  - wait for all customers and cooks to be done
 *  - close the restaurant.
 */
int main() {
		bcb = OpenRestaurant(BENSCHILLIBOWL_SIZE, EXPECTED_NUM_ORDERS);

		pthread_t customer[NUM_CUSTOMERS];
		int customer_id[NUM_CUSTOMERS];
		pthread_t cook[NUM_COOKS];
		int cook_id[NUM_COOKS];

		int i;
		for(i=0; i<NUM_COOKS; i++){
			cook_id[i] = i+1;
			pthread_create(&cook[i], NULL, &BENSCHILLIBOWLCook, (void*) &(cook_id[i]));
		}

		for(i=0; i<NUM_CUSTOMERS; i++){
			customer_id[i] = i+1;
			pthread_create(&customer[i], NULL, &BENSCHILLIBOWLCustomer, (void*) &(customer_id[i]));
		}

		for(i=0; i<NUM_CUSTOMERS; i++){
			pthread_join(customer[i], NULL);
		}

		for(i=0; i<NUM_COOKS; i++){
			pthread_join(cook[i], NULL);
		}

		CloseRestaurant(bcb);
    return 0;
}