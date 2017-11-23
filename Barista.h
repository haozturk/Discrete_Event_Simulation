#ifndef tonyProj_BARISTA_H
#define tonyProj_BARISTA_H


#include "Order.h"

class Barista {

public:

    int BaristaNum;
    double busyTime;
    Barista(int, double);

    Order* currentOrderptr; //the order that the barista is busy with
    //Order currentOrder;
    bool isEmpty;
    long maxLength2ndModel;

};


#endif //CMPE250_PROJECT2_BARISTA_H
