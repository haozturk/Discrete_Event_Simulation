#include <iostream>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <iterator>
#include <vector>
#include <queue>
#include <ctime>
#include "Order.h"
#include "Cashier.h"
#include "Barista.h"

using namespace std;

struct sortByOrderDoneTime
{
    inline bool operator() (const Order* struct1, const Order* struct2)
    {
        return ((*(struct1)).orderDoneTime < (*(struct2)).orderDoneTime);
    }
};

template <class Container>
void split1(const string& str, Container& cont)
{
    istringstream iss(str);
    copy(istream_iterator<string>(iss),
         istream_iterator<string>(),
         back_inserter(cont));
}

int main(int argc, char* argv[]) {


    std::clock_t start;
    double duration;

    start = std::clock();
    // below reads the input file
    // in your next projects, you will implement that part as well
    if (argc != 3) {
        cout << "Run the code with the following command: ./project1 [input_file] [output_file]" << endl;
        return 1;
    }

    ifstream infile(argv[1]);
    string line;
    vector<string> input;
    // process first line
    getline(infile, line);
    int cashierNum = stoi(line);
    cout << "number of cashiers: " << cashierNum << endl;
    vector<Cashier> cashierVec;
    for(int i=0;i<cashierNum;i++){

        Cashier newCashier(i+1,0);
        cashierVec.push_back(newCashier);

    }
    vector<Barista> baristaVec;
    int baristaNum = cashierNum/3;

    for(int i=0;i<cashierNum/3;i++){

        Barista newBarista(i+1,0);
        baristaVec.push_back(newBarista);

    }

    getline(infile,line);
    int orderNum = stoi(line);
    cout << "number of orders: " << orderNum << endl;
    vector<Order*> orderVec; // a vector which holds the orders

    //The following code generates the orderVec dynamicaly using the input file
    Order* p;
    for (int i=0; i<orderNum; i++) {
        getline(infile, line);
        vector<string> words;
        split1(line,words);
        p = new Order(stod(words[0]),stod(words[1]),stod(words[2]),stod(words[3]));
        orderVec.push_back(p);
    }

    //creates queues for baristas and stores them in a vector for 2nd model
    vector<queue<Order*>*> secondModelVec;
    for(int i=0;i<baristaNum;i++){
        queue<Order*>* q = new queue<Order*>;
        secondModelVec.push_back(q);
    }

    long maxLengthOfCashierQueue = 0; //for both 1st model and 2nd model
    long maxLengthOfBaristaQueue = 0; //for 1st model
    //These 2 queues are used to simulate the queues for cashier and barista queues
    queue<Order *> tempQueueForCashiers;
    queue<Order *> tempQueueForBaristas;
    //fills the cashier queue
    for (int i = 0; i < orderNum; i++) {
        tempQueueForCashiers.push(orderVec[i]);
    }

    /**************************CASHIER QUEUE BEGINS FOR 1ST AND 2ND MODEL*************************/
    std::clock_t start2;
    double duration2;

    start2 = std::clock();

    bool orderTaken = 0;
    while (!tempQueueForCashiers.empty())
    {

        bool orderProcessed = false; //becomes true if the current order is put to an empty cashier
        bool thereIsAnEmptyCashier = false;
        int indexOfTheEmptySpot=0;
        //this loop detects the empty cashier with the earliest index
        for (int j = 0; j < cashierNum; j++) {
            if (cashierVec[j].isEmpty) {
                indexOfTheEmptySpot = j;
                thereIsAnEmptyCashier = true;
                break;
            }
        }
        //the following if block puts the current order to the empty spot if there is one
        if(thereIsAnEmptyCashier){
            cashierVec[indexOfTheEmptySpot].isEmpty = 0;
            (*(tempQueueForCashiers.front())).orderDoneTime = (*(tempQueueForCashiers.front())).arrivalTime + (*(tempQueueForCashiers.front())).orderTime;
            cashierVec[indexOfTheEmptySpot].busyTime += (*(tempQueueForCashiers.front())).orderTime;
            cashierVec[indexOfTheEmptySpot].currentOrderptr = tempQueueForCashiers.front();
            //Every order has a cashIndex field which specifies in which cashier it is processed
            //This index is used to determine in which barista queue for 2nd Model we will put it
            //if you divide cashIndex by 3 , you'll get the index of the queue where the order will be put
            (*(tempQueueForCashiers.front())).cashIndex = indexOfTheEmptySpot; // 2ndModel index
            tempQueueForCashiers.pop();
            orderProcessed = true;
        }

        //if an order is put to an empty spot do not enter this loop, instead go back to while and process the next order
        if (!orderProcessed) {
            //Following code finds the cashier that becomes available earliest
            int indexOfEarliestEmptyCashier = 0;  //this field is used if the arrival time of the order in front of the queue is smaller than the orderDoneTimes of all cashiers
            double minOrderDoneTime = (*(cashierVec[0].currentOrderptr)).orderDoneTime;
            for (int i = 1; i < cashierNum; i++) {
                if ((*(cashierVec[i].currentOrderptr)).orderDoneTime < minOrderDoneTime) {
                    minOrderDoneTime = (*(cashierVec[i].currentOrderptr)).orderDoneTime;
                    indexOfEarliestEmptyCashier = i;
                }
            }

            vector<Order*> tempVectorForSorting; //this vector is created to sort the orders that are being processed
            //refresh the cashiers according to the arrival time of the current order
            for(int j = 0; j < cashierNum;j++){
                if((*(tempQueueForCashiers.front())).arrivalTime > (*(cashierVec[j].currentOrderptr)).orderDoneTime){
                    cashierVec[j].isEmpty = true;
                    tempVectorForSorting.push_back(cashierVec[j].currentOrderptr); //this for the Barista queue
                }
            }

            //This 5 lines of code sorts the orders that are freed after the refreshment and pushs them to the barista queue
            sort(tempVectorForSorting.begin(),tempVectorForSorting.end(),sortByOrderDoneTime());
            for(int i=0;i<tempVectorForSorting.size();i++){
                tempQueueForBaristas.push(tempVectorForSorting[i]);
            }
            tempVectorForSorting.clear();

            //places the order in front of the queue to the cashier with the earliest index after refreshment
            for(int j = 0; j < cashierNum;j++){
                if(cashierVec[j].isEmpty){
                    cashierVec[j].isEmpty = 0;
                    (*(tempQueueForCashiers.front())).orderDoneTime = (*(tempQueueForCashiers.front())).arrivalTime + (*(tempQueueForCashiers.front())).orderTime;
                    cashierVec[j].currentOrderptr = tempQueueForCashiers.front();
                    cashierVec[j].busyTime += (*(tempQueueForCashiers.front())).orderTime;
                    (*(tempQueueForCashiers.front())).cashIndex = j; // 2ndModel index
                    orderTaken = 1;
                    break;
                }
            }
            //means that arrival time of the order in front of the queue is larger orderDoneTimes of some of the cashiers
            //in this case we placed it to the empty cashier with the smallest index according to its arrival time
            if(orderTaken){
                orderTaken=0;
                tempQueueForCashiers.pop();
            }
                //if the order waits in the queue means that it enters a queue
            else {
                //The following 2 for loop and 1 if statement is there to find the maximum length of the cashier queue
                long currentQueueSize = 0; // this is for max length
                vector<Order *> tempVectorForTraversingTheQueue;
                long sizeOfTheQueue = tempQueueForCashiers.size(); // this is for loop count
                for (int i = 0; i < sizeOfTheQueue; i++) {
                    Order *front = tempQueueForCashiers.front();
                    tempVectorForTraversingTheQueue.push_back(front);
                    tempQueueForCashiers.pop();
                }
                for (int i = 0; i < tempVectorForTraversingTheQueue.size(); i++) {
                    if ((*(tempVectorForTraversingTheQueue[i])).arrivalTime <
                        (*(cashierVec[indexOfEarliestEmptyCashier].currentOrderptr)).orderDoneTime) {
                        currentQueueSize++;
                    } else {
                        break;
                    }
                }
                if (currentQueueSize > maxLengthOfCashierQueue) {
                    maxLengthOfCashierQueue = currentQueueSize;
                }
                //copy the vector back to the queue
                for (int i = 0; i < tempVectorForTraversingTheQueue.size(); i++) {
                    tempQueueForCashiers.push(tempVectorForTraversingTheQueue[i]);
                }
                tempVectorForTraversingTheQueue.clear();

                //this block handles the replacement part
                (*(tempQueueForCashiers.front())).orderDoneTime = (*(tempQueueForCashiers.front())).orderTime +
                                                                  (*(cashierVec[indexOfEarliestEmptyCashier].currentOrderptr)).orderDoneTime; //The difference
                tempQueueForBaristas.push(cashierVec[indexOfEarliestEmptyCashier].currentOrderptr); // for BaristaQueue
                (*(tempQueueForCashiers.front())).cashIndex = indexOfEarliestEmptyCashier; // 2ndModel index
                //(*(secondModelVec[((*(tempQueueForCashiers.front())).cashIndex)/3])).push(tempQueueForCashiers.front()); // 2ndModel push
                cashierVec[indexOfEarliestEmptyCashier].busyTime += (*(tempQueueForCashiers.front())).orderTime;
                cashierVec[indexOfEarliestEmptyCashier].currentOrderptr = tempQueueForCashiers.front();
                tempQueueForCashiers.pop();
            }
        }
    }
    //This additional part pushes the orders to the barista queue that are left in the cashiers according to their
    //orderDoneTimes
    vector<Order*> tempVectorForSorting;
    for(int j = 0; j < cashierNum;j++){
        tempVectorForSorting.push_back(cashierVec[j].currentOrderptr); //this for the Barista queue
    }
    //This 5 lines of code sorts the order that are freed after the refreshment and pushs them to the barista queue
    sort(tempVectorForSorting.begin(),tempVectorForSorting.end(),sortByOrderDoneTime());
    for(int i=0;i<tempVectorForSorting.size();i++){
        tempQueueForBaristas.push(tempVectorForSorting[i]);
        //(*(secondModelVec[((*(tempVectorForSorting[i])).cashIndex)/3])).push(tempVectorForSorting[i]); // 2ndModel push
    }
    tempVectorForSorting.clear();

    //copying the queue to a vector to copy it to another vector for 2nd model later
    vector<Order*> tempVect;
    int length = tempQueueForBaristas.size();
    for(int i=0;i<length;i++){
        Order* ptr =tempQueueForBaristas.front();
        tempVect.push_back(ptr);
        tempQueueForBaristas.pop();
    }
    for(int i=0;i<tempVect.size();i++){
        tempQueueForBaristas.push(tempVect[i]);
    }

    duration2 = ( std::clock() - start2 ) / (double) CLOCKS_PER_SEC;

    std::cout<<"1st loop takes : "<< duration2 << " seconds" << endl;

    /*****************************CASHIER QUEUE IS PROCESSED*****************************************/

    /**************************BARISTA QUEUE BEGINS FOR 1ST MODEL************************************/
    std::clock_t start3;
    double duration3;

    start3 = std::clock();

    bool orderTaken2 = 0;
    while (!tempQueueForBaristas.empty())
    {

        bool orderProcessed = false;
        bool thereIsAnEmptyBarista = false;
        int indexOfTheEmptySpot=0;
        //if there is an available barista, fill it with the element in front of the queue
        for (int j = 0; j < baristaNum; j++) {
            if (baristaVec[j].isEmpty) {
                indexOfTheEmptySpot = j;
                thereIsAnEmptyBarista = true;
                break;
            }
        }
        if(thereIsAnEmptyBarista){
            baristaVec[indexOfTheEmptySpot].isEmpty = 0;
            (*(tempQueueForBaristas.front())).brewDoneTime = (*(tempQueueForBaristas.front())).orderDoneTime + (*(tempQueueForBaristas.front())).brewTime;
            baristaVec[indexOfTheEmptySpot].busyTime += (*(tempQueueForBaristas.front())).brewTime;
            baristaVec[indexOfTheEmptySpot].currentOrderptr = tempQueueForBaristas.front();
            tempQueueForBaristas.pop();
            orderProcessed = true;
        }

        //if an order is put to an empty spot do not enter this loop, instead go back to while
        if (!orderProcessed) {
            //Following code finds the barista that becomes available earliest
            int indexOfEarliestEmptyBarista = 0;  //this field is used if the arrival time of the order in front of the queue is smaller than the brewDoneTimes of all baristas
            double minBrewDoneTime = (*(baristaVec[0].currentOrderptr)).brewDoneTime;
            for (int i = 1; i < baristaNum; i++) {
                if ((*(baristaVec[i].currentOrderptr)).brewDoneTime < minBrewDoneTime) {
                    minBrewDoneTime = (*(baristaVec[i].currentOrderptr)).brewDoneTime;
                    indexOfEarliestEmptyBarista = i;
                }
            }

            //refresh the baristas according to the arrival time of the current order
            for(int j = 0; j < baristaNum;j++){
                if((*(tempQueueForBaristas.front())).orderDoneTime > (*(baristaVec[j].currentOrderptr)).brewDoneTime){
                    baristaVec[j].isEmpty = true;
                }
            }
            //places the order in front of the queue to the barista with the earliest index after refreshment
            for(int j = 0; j < baristaNum;j++){
                if(baristaVec[j].isEmpty){
                    baristaVec[j].isEmpty = 0;
                    (*(tempQueueForBaristas.front())).brewDoneTime = (*(tempQueueForBaristas.front())).orderDoneTime + (*(tempQueueForBaristas.front())).brewTime;
                    baristaVec[j].currentOrderptr = tempQueueForBaristas.front();
                    baristaVec[j].busyTime += (*(tempQueueForBaristas.front())).brewTime;
                    orderTaken2 = 1;
                    break;
                }
            }
            //means that arrival time of the order in front of the queue is larger brewDoneTimes of some of the baristas
            //in this case we placed it to the empty barista with the smallest index according to its arrival time
            if(orderTaken2){
                orderTaken2=0;
                tempQueueForBaristas.pop();
            }
                //if the order waits in the queue means that it enters a queue
            else {
                //The following 2 for loop and 1 if statement is there to find the maximum length of the barista queue
                long currentQueueSize = 0; // this is for max length
                vector<Order *> tempVectorForTraversingTheQueue;
                long sizeOfTheQueue = tempQueueForBaristas.size(); // this is for loop count
                for (int i = 0; i < sizeOfTheQueue; i++) {
                    Order *front = tempQueueForBaristas.front();
                    tempVectorForTraversingTheQueue.push_back(front);
                    tempQueueForBaristas.pop();
                }
                for (int i = 0; i < tempVectorForTraversingTheQueue.size(); i++) {
                    if ((*(tempVectorForTraversingTheQueue[i])).orderDoneTime <
                        (*(baristaVec[indexOfEarliestEmptyBarista].currentOrderptr)).brewDoneTime) {
                        currentQueueSize++;
                    } else {
                        break;
                    }
                }
                if (currentQueueSize > maxLengthOfBaristaQueue) {
                    maxLengthOfBaristaQueue = currentQueueSize;
                }
                //The following for loop finds the index of the order which has the maximum price
                double maxPrice = 0;
                int maxPriceIndex = 0; //index of the order which has the maximum price in the current queue
                for(int i=0;i<currentQueueSize;i++){
                    if((*(tempVectorForTraversingTheQueue[i])).price > maxPrice){
                        maxPrice = (*(tempVectorForTraversingTheQueue[i])).price;
                        maxPriceIndex = i;
                    }
                }
                //copy the vector back to the queue
                for (int i = 0; i < tempVectorForTraversingTheQueue.size(); i++) {
                    //copy all the elements to the queue except the one with the max price
                    if(i != maxPriceIndex){
                        tempQueueForBaristas.push(tempVectorForTraversingTheQueue[i]);
                    }
                }

                //this block handles the replacement part
                (*(tempVectorForTraversingTheQueue[maxPriceIndex])).brewDoneTime = (*(tempVectorForTraversingTheQueue[maxPriceIndex])).brewTime +
                                                                                   (*(baristaVec[indexOfEarliestEmptyBarista].currentOrderptr)).brewDoneTime; //The difference
                baristaVec[indexOfEarliestEmptyBarista].busyTime += (*(tempVectorForTraversingTheQueue[maxPriceIndex])).brewTime;
                baristaVec[indexOfEarliestEmptyBarista].currentOrderptr = tempVectorForTraversingTheQueue[maxPriceIndex];
                tempVectorForTraversingTheQueue.clear();
            }
        }
    }
    duration3 = ( std::clock() - start3 ) / (double) CLOCKS_PER_SEC;

    std::cout<<"2nd loop takes: "<< duration3 << " seconds" << endl;

    /********************BARISTA QUEUE FOR 1ST MODEL IS PROCESSED**************************************/


    double totalRunningTime1stModel = 0;

    for(int i =0;i<orderNum;i++){
        if((*(orderVec[i])).brewDoneTime > totalRunningTime1stModel){
            totalRunningTime1stModel = (*(orderVec[i])).brewDoneTime;
        }
    }
    //File writings for the 1st model
    FILE *myfile;
    myfile = fopen(argv[2],"w");

    fprintf(myfile,"%.2lf\n",totalRunningTime1stModel);
    fprintf(myfile,"%lu\n",maxLengthOfCashierQueue);
    fprintf(myfile,"%lu\n",maxLengthOfBaristaQueue);
    //utilizations of cashiers
    for(int i=0;i<cashierNum;i++){
        double util=cashierVec[i].busyTime/totalRunningTime1stModel;
        fprintf(myfile,"%.2lf\n",util);
    }
    //utilizations of baristas
    for(int i=0;i<baristaNum;i++){
        double util=baristaVec[i].busyTime/totalRunningTime1stModel;
        fprintf(myfile,"%.2lf\n",util);
    }
    //Turnaround times of order
    for(int i=0;i<orderNum;i++){
        fprintf(myfile,"%.2lf\n",orderVec[i]->brewDoneTime - orderVec[i]->arrivalTime);
    }


    /************PREPARATION FOR 2ND MODEL**********************/
    //The necessary vector is constituted from tempVect
    for(int i=0;i<orderNum;i++){
        (*(secondModelVec[((*(tempVect[i])).cashIndex)/3])).push(tempVect[i]);
    }
    //the loop for 1st model is above, therefore we need to update the baristas to their default values
    for(int i=0;i<baristaNum;i++){
        baristaVec[i].isEmpty=1;
        baristaVec[i].busyTime = 0;
        Order o(0,0,0,0);
        Order* p=&o;
        baristaVec[i].currentOrderptr = p;
    }
    //update the brewDoneTimes of the orders
    for(int i=0;i<orderNum;i++){
        orderVec[i]->brewDoneTime=0;
    }

    /******************PREPARATION FOR 2ND MODEL IS DONE***********/

    /******************BARISTA QUEUE FOR 2ND MODEL BEGINS*********************************/

    std::clock_t start4;
    double duration4;

    start4 = std::clock();

    for(int t=0;t<baristaNum;t++){

        bool orderTaken2 = 0;
        while (!(*(secondModelVec[t])).empty())
        {

            bool orderProcessed = false;
            //if the current barista is empty, place the order to it
            if(baristaVec[t].isEmpty){
                baristaVec[t].isEmpty = 0;
                (*((*(secondModelVec[t])).front())).brewDoneTime = (*((*(secondModelVec[t])).front())).orderDoneTime + (*((*(secondModelVec[t])).front())).brewTime;
                baristaVec[t].busyTime += (*((*(secondModelVec[t])).front())).brewTime;
                baristaVec[t].currentOrderptr = (*(secondModelVec[t])).front();
                (*(secondModelVec[t])).pop();
                orderProcessed = true;
            }

            //if an order is put to an empty spot do not enter this loop, instead go back to while
            if (!orderProcessed) {
                //refresh the barista
                if( (*((*(secondModelVec[t])).front())).orderDoneTime > (*(baristaVec[t].currentOrderptr)).brewDoneTime){
                    baristaVec[t].isEmpty = true;
                }
                //places the order after refreshment
                if(baristaVec[t].isEmpty){
                    baristaVec[t].isEmpty = 0;
                    (*((*(secondModelVec[t])).front())).brewDoneTime = (*((*(secondModelVec[t])).front())).orderDoneTime + (*((*(secondModelVec[t])).front())).brewTime;
                    baristaVec[t].currentOrderptr = (*(secondModelVec[t])).front();
                    baristaVec[t].busyTime += (*((*(secondModelVec[t])).front())).brewTime;
                    orderTaken2 = 1;
                }

                //means that arrival time of the order in front of the queue is larger orderDoneTimes of some of the cashiers
                //in this case we placed it to the the empty cashier with the smallest index according to its arrival time
                if(orderTaken2){
                    orderTaken2=0;
                    (*(secondModelVec[t])).pop();
                }
                    //if the order waits in the queue means that it enters a queue
                else {
                    //The following 2 for loop and 1 if statement is there to find the maximum length of the cashier queue
                    long currentQueueSize = 0; // this is for max length
                    vector<Order *> tempVectorForTraversingTheQueue;
                    long sizeOfTheQueue = (*(secondModelVec[t])).size(); // this is for loop count
                    for (int i = 0; i < sizeOfTheQueue; i++) {
                        Order *front = (*(secondModelVec[t])).front();
                        tempVectorForTraversingTheQueue.push_back(front);
                        (*(secondModelVec[t])).pop();
                    }
                    for (int i = 0; i < tempVectorForTraversingTheQueue.size(); i++) {
                        if ((*(tempVectorForTraversingTheQueue[i])).orderDoneTime <
                            (*(baristaVec[t].currentOrderptr)).brewDoneTime) {
                            currentQueueSize++;
                        } else {
                            break;
                        }
                    }
                    if (currentQueueSize > baristaVec[t].maxLength2ndModel) {
                        baristaVec[t].maxLength2ndModel = currentQueueSize;
                    }
                    //The following for loop finds the index of the order which has the maximum price
                    double maxPrice = 0;
                    int maxPriceIndex = 0; //index of the order which has the maximum price in the current queue
                    for(int i=0;i<currentQueueSize;i++){
                        if((*(tempVectorForTraversingTheQueue[i])).price > maxPrice){
                            maxPrice = (*(tempVectorForTraversingTheQueue[i])).price;
                            maxPriceIndex = i;
                        }
                    }
                    //copy the vector back to the queue
                    for (int i = 0; i < tempVectorForTraversingTheQueue.size(); i++) {
                        //copy all the elements to the queue except the one with the max price
                        if(i != maxPriceIndex){
                            (*(secondModelVec[t])).push(tempVectorForTraversingTheQueue[i]);
                        }
                    }

                    //this block handles the replacement part
                    (*(tempVectorForTraversingTheQueue[maxPriceIndex])).brewDoneTime = (*(tempVectorForTraversingTheQueue[maxPriceIndex])).brewTime +
                                                                                       (*(baristaVec[t].currentOrderptr)).brewDoneTime; //The difference
                    baristaVec[t].busyTime += (*(tempVectorForTraversingTheQueue[maxPriceIndex])).brewTime;
                    baristaVec[t].currentOrderptr = tempVectorForTraversingTheQueue[maxPriceIndex];
                    tempVectorForTraversingTheQueue.clear();
                }
            }
        }
    }

    duration4 = ( std::clock() - start4 ) / (double) CLOCKS_PER_SEC;

    std::cout<<"3rd loop takes: "<< duration4 << " seconds" << endl;

    /****************BARISTA QUEUE FOR 2ND MODEL IS PROCESSED*****************************/

    double totalRunningTime2ndModel =0;
    for(int i =0;i<orderNum;i++){
        if((*(orderVec[i])).brewDoneTime > totalRunningTime2ndModel){
            totalRunningTime2ndModel = (*(orderVec[i])).brewDoneTime;
        }
    }

    fprintf(myfile,"\n");
    fprintf(myfile,"%.2lf\n",totalRunningTime2ndModel);
    fprintf(myfile,"%lu\n",maxLengthOfCashierQueue);
    for(int i=0;i<baristaNum;i++){
        fprintf(myfile,"%lu\n",baristaVec[i].maxLength2ndModel);
    }
    //utilizations of cashiers
    for(int i=0;i<cashierNum;i++){
        double util=cashierVec[i].busyTime/totalRunningTime2ndModel;
        fprintf(myfile,"%.2lf\n",util);
    }
    //utilizations of baristas
    for(int i=0;i<baristaNum;i++){
        double util=baristaVec[i].busyTime/totalRunningTime2ndModel;
        fprintf(myfile,"%.2lf\n",util);
    }
    //Turnaround times of order
    for(int i=0;i<orderNum;i++){
        fprintf(myfile,"%.2lf\n",orderVec[i]->brewDoneTime - orderVec[i]->arrivalTime);
    }
    fclose(myfile);

    for(int i=0;i<orderNum;i++){
        delete orderVec[i];
    }
    for(int i=0;i<baristaNum;i++){
        delete secondModelVec[i];
    }

    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;

    std::cout<<"Main process takes: "<< duration << " seconds" << endl;

    return 0;
}
