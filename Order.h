#ifndef tonyProj_ORDER_H
#define tonyProj_ORDER_H



class Order {

public:
    double arrivalTime;
    double orderTime;
    double brewTime;
    double price;
    Order();
    Order(double, double, double, double);

    double orderDoneTime; //the time when the order finishes the order giving process
    double brewDoneTime; //the time when the brewing process finishes
    int cashIndex; //the cashier who takes that order



};


#endif