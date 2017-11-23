#ifndef tonyProj_CASHIER_H
#define tonyProj_CASHIER_H


#include "Order.h"

class Cashier {

public:
    int priorityNum;
    double busyTime;
    Cashier(int, double);

    Order* currentOrderptr;
    //Order currentOrder;
    bool isEmpty;

};


#endif //CMPE250_PROJECT2_CASHIER_H
