#include "Barista.h"

Barista::Barista(int baristaNum, double busyTime) {

    this->BaristaNum=baristaNum;
    this->busyTime=busyTime;

    this->isEmpty = true;
    this->currentOrderptr = nullptr;
    this->maxLength2ndModel=0;

}
