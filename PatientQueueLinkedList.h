
/*
 * File: LinkedListPatientQueue.h
 * ------------------------------
 *  Name: Silvia Fernandez (SUNet ID: silviaf)
 * This file declares a set of methods and an instance variable for the class VectorPatientQueue.
 * (Singly linked list implementation of class).
 * This file does not include any extension functionality. Please refer to the last submission of this
 * homework in paperless for extension functionality.
 */
#pragma once
#include <iostream>
#include <string>
#include "patientnode.h"
#include "patientqueue.h"
using namespace std;
 
class LinkedListPatientQueue : public PatientQueue {
public:
    LinkedListPatientQueue();
    ~LinkedListPatientQueue();
    string frontName();
    void clear();
    int frontPriority();
    bool isEmpty();
    void newPatient(string name, int priority);
    string processPatient();
    void upgradePatient(string name, int newPriority);
    string toString();
    void remove(int index);
 
private:
    PatientNode *head;
};