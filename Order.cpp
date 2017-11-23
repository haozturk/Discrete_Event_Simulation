#include <iostream>
#include "Order.h"

using namespace std;

Order::Order() {}

Order::Order(double arr, double ord, double brew, double price ) {

    this->arrivalTime = arr;
    this->orderTime = ord;
    this->brewTime = brew;
    this->price = price;

    this->orderDoneTime = 0;
    this->brewDoneTime = 0;
    this->cashIndex =0;
}



