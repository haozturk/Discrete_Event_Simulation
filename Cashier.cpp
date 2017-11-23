#include "Cashier.h"

Cashier::Cashier(int priority, double busy){

    this->priorityNum = priority;
    this->busyTime = busy;

    //Order nullOrder(0,0,0,0);

    this->isEmpty = true;
    this->currentOrderptr = nullptr; //?
    //this->currentOrder = nullOrder;
};