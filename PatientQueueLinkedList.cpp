*
 * File: LinkedListPatientQueue.cpp
 * --------------------------
 * Name: Silvia Fernandez (SUNet ID: silviaf)
 * This file implements the LinkedListPatientQueue class using a singly linked list. The
 * LinkedListPatientQueue class includes the following methods:
 * - LinkedListPatientQueue constructor
 * - LinkedListPatientQueue destructor
 * - method to clear the patient queue which also frees memory
 * - method to return the name of the most urgent patient or an exception if the patient queue
 * is empty.
 * - method to return the most urgent priority or an exception if the patient queue
 * is empty.
 * - method to check if the patient queue is empty.
 * - method to add a patient with a given priority to the patient queue.
 * - method to remove the patient with most urgent priority from queue and return their name.
 * - method to print the patient queue to the console.
 * This file does not include any extension functionality. Please refer to the last submission of this
 * homework in paperless for extension functionality.
 */
 
#include "LinkedListPatientQueue.h"
#include "linkedlist.h"
#include "strlib.h"
 
/* Text for string exceptions */
const string EXCEPTION_QUEUE_EMPTY = "Patient queue is empty";
const string EXCEPTION_PATIENT_NOT_FOUND = "Patient was not found in queue";
const string EXCEPTION_WRONG_PRIORITY = "Patient priorities cannot be downgraded";
 
/** Method: LinkedListPatientQueue
 * Usage: LinkedListPatientQueue()
 * -----------------------------------------------------
 * Instantiates one LinkedListPatientQueue object.
 */
LinkedListPatientQueue::LinkedListPatientQueue() {
    head = NULL;
}
 
/** Method: ~LinkedListPatientQueue
 * -----------------------------------------------------
 * Frees up any memory used - when the class instance goes out of scope the destructor is called
 * cleaning up memory for us.
 */
LinkedListPatientQueue::~LinkedListPatientQueue() {
    clear();
}
 
/** Method: clear
 * Usage: clear()
 * -------------------------------------------------------
 * Removes all elements from the patient queue & frees memory.
 */
void LinkedListPatientQueue::clear() {
    PatientNode *currentNode = head;
    while (currentNode != NULL) {
        PatientNode *tempNode = currentNode -> next;
        delete currentNode;
        currentNode = tempNode;
    }
    head = NULL;
}
 
/** Method: frontName
 * Usage: frontName()
 * -----------------------------------------------------
 * Returns the name of the most urgent patient (the person in the front of our patient queue,
 * with lowest priority score), without removing it or altering the state of the queue.
 * Since our singly linked list stores patients in order of priority, this is
 * equivalent to returning the name of the patient at the front of the linked list.
 * If there is a tie (if 2+ patients have the same priority) the patient who arrived first
 * (i.e. who was added first to the patient queue) from those with the highest priority
 * (lowest priority score) will be at the front of the singly linked list and will be returned.
 * Throws a string exception if the queue does not contain any patients.
 * @return mostUrgentPatient: type string, name of the most urgent patient.
 */
string LinkedListPatientQueue::frontName() {
    if (isEmpty()) {
        throw EXCEPTION_QUEUE_EMPTY;
    } else {
        return head -> name;
    }
}
 
/** Method: frontPriority
 * Usage: frontPriority()
 * ----------------------------------------------------
 * Returns the integer priority of the most urgent patient (the person in the front of the patient queue),
 * without removing the patient or altering the state of the queue. Throws a string exception if the queue
 * does not contain any patients. Since our singly linked list stores patients in order of priority,
 * this corresponds to returning the priority of the first element in the linked list.
 * @return mostUrgentPriority: type int, priority of the most urgent patient.
 */
int LinkedListPatientQueue::frontPriority() {
    if (isEmpty()) {
        throw EXCEPTION_QUEUE_EMPTY;
    } else {
        return head -> priority;
    }
}
 
/** Method: isEmpty
 * Usage: isEmpty()
 * --------------------------------------------------
 * Returns true if the patient queue does not contain any patients, false otherwise.
 */
bool LinkedListPatientQueue::isEmpty() {
    return (head == NULL);
}
 
/** Method: newPatient
 * Usage: newPatient()
 * --------------------------------------------------
 * Adds to the patient queue a patient of name the string passed as parameter into the method with
 * the priority being the integer passed as parameter to the method. Patients are stored by order of
 * priority from most urgent patient (lowest priority score) to least urgent patient. If 2 adjacent
 * patients have the same priority scores, the patient closest to the head of the linked list has
 * higher priority as he/she arrived sooner.
 * Duplicate names and priorities are allowed in our patient queue. Patient names are stored preserving
 * the format of the string passed to this method.
 * @param: name - string type. Name of the new patient that will be added to the patient queue.
 * @param: priority - int type. Priority of the new patient that will be added to the patient queue.
 */
void LinkedListPatientQueue::newPatient(string name, int priority) {
    if (isEmpty()) {
        PatientNode *newPatient = new PatientNode(name, priority, NULL);
        head = newPatient;
    } else {
        PatientNode *currentNode = head;
        PatientNode *prevNode = NULL;
        while (currentNode != NULL) {
            // if we find a patient in the patient list with a higher priority than our new patient
            if (priority < currentNode -> priority) {
                if (prevNode != NULL) {
                    /* If we're not at the head of the queue, insert new patient before the patient
                     * with higher priority*/
                    prevNode -> next = new PatientNode(name, priority, currentNode);
                    return;
                } else {
                    // if we are at the beginning of the queue, insert the new patient there
                    head = new PatientNode(name, priority, currentNode);
                    return;
                }
            }
            prevNode = currentNode;
            currentNode = currentNode -> next;
        }
        /* if we reach the end of the patient list and we have not added the new patient yet,
         * we add it.
         */
        prevNode -> next = new PatientNode(name, priority, NULL);
    }
}
 
/** Method: processPatient
 * Usage: processPatient()
 * ----------------------------------------------------
 * Removes the patient with most urgent priority from queue and returns their name.
 * Throws a string exception in the list is empty. Since patients in the singly linked list
 * are ordered by decreasing priority (increasing priority score) this method removes
 * the first patient in our patient queue and returns the name of the first patient in our patient
 * queue.
 * @return: patientProcessedName - type String. Name of the patient that was processed.
 */
string LinkedListPatientQueue::processPatient() {
    if (isEmpty()) {
        throw EXCEPTION_QUEUE_EMPTY;
    } else {
        string toReturn = head -> name;
        PatientNode *temp = head -> next;
        delete head;
        head = temp;
        return toReturn;
    }
}
 
/** Method: remove()
 * Usage: remove(index)
 * -----------------------------
 * Removes from the patient queue the patient at the index passed as parameter to this method. I
 * @param: index type int. Index where we're removing the patient.
 */
void LinkedListPatientQueue::remove (int index) {
    if (index == 0) {
        PatientNode *temp = head -> next;
        delete head;
        head = temp;
        return;
    }
    int counter = 0;
    PatientNode *currentNode = head;
    PatientNode *nextNode = head -> next;
    while (counter < index - 1) {
        currentNode = nextNode;
        nextNode = nextNode -> next;
        counter++;
    }
    // nextNode pointing to index, currentNode pointing to the previous one
    currentNode -> next = nextNode -> next;
    delete nextNode;
}
 
 
/** Method: upgradePatient
 * Usage: upgradePatient()
 * -----------------------------------------------------
 * Modifies the priority of a given existing patient in the queue. Changes the patient's priority to
 * a more urgent value than its current value (i.e. lower priority score).
 * Throws a string exception if the given patient is present in the queue and has a more urgent
 * priority than the tentative new priority, or if the given patient is not already in the
 * queue. If there are multiple patients with the same name in the queue, the patient with the
 * hightest priority has his/her priority updated. This is equivalent to
 * upgrading the first patient of the patient queue that has the name we're passing as parameter to
 * this method.
 * @param: name, type string. Name of the patient we want to update the priority for.
 * @param: newPriority, type int. New priority for the patient we want to update the priority for.
 */
void LinkedListPatientQueue::upgradePatient(string name, int newPriority) {
    PatientNode *currentNode = head;
    int counter = 0;
    while (currentNode != NULL) {
        if (currentNode -> name == name) {
            if (currentNode -> priority < newPriority) {
                throw EXCEPTION_WRONG_PRIORITY;
            } else if (currentNode -> priority == newPriority) {
                return;
            } else {
                remove(counter);
                newPatient(name, newPriority);
                return;
            }
        } else {
            currentNode = currentNode -> next;
            counter++;
        }
    }
    throw EXCEPTION_PATIENT_NOT_FOUND;
}
 
 
 
 
 
/** Method: toString
 * Usage: toString()
 * --------------------------------------------------------
 * Returns a string that is used to print the patient queue to the console. String returned is
 * as follows: {} braces wrapping the string, and elements of the patient queue are printed out
 * in front-to-back order. Each element is in the form of priority:patient name with each element
 * being separated by a comma and space.
 * @return: string containing elements in patient queue in the format specified above.
 */
string LinkedListPatientQueue::toString() {
    string toReturn = "{";
    PatientNode *currentNode = head;
    while (currentNode != NULL) {
        toReturn += integerToString(currentNode -> priority) + ":" +
                currentNode -> name;
        if (currentNode -> next != NULL) {
            toReturn += ", ";
        }
        currentNode = currentNode->next;
    }
    toReturn += "}";
    return toReturn;
}
 
 
    