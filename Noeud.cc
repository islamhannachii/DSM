#include <Noeud.h>

using namespace omnetpp;

void Noeud::initialize() {
    //remplir la table locale avec des valeurs aleatoires entre 0-100, selon l'index du noeud
    int minVal = 0;
    int maxVal = 100;
    par("localTab").setStringValue(
            remplirLocalTab(par("localTab"), getIndex(), minVal, maxVal));
    par("reponsesRestees").setIntValue(0);
    par("somme").setIntValue(0);
    EV << "Noeud " << getIndex()<< ": ma table locale: " << par("localTab") << endl;
    if (getIndex() == 3) {
        scheduleAt(simTime() + par("d").doubleValue(), new cMessage("calcul1"));
    }
    if (getIndex() == 1) {
        scheduleAt(simTime() + par("d").doubleValue(), new cMessage("calcul2"));
    }
}
void Noeud::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage() && getIndex()==3) {
        par("reponsesRestees").setIntValue(15); // 19 éléments - 4 locaux
        for (int i = 0; i < 19; i++) {
            if (i >= 12 && i < 16) {
                par("somme").setIntValue(
                        par("somme").intValue()
                            + stringToIntVector(par("localTab"), ' ')[i]);
            } else {
                getOwnerNoeudRequest(i, 'r');
            }
        }
    } else {
        if (msg->isSelfMessage() && getIndex() ==1)
        {
            par("reponsesRestees").setIntValue(12); // 12 éléments
            for (int i = 8; i < 20; i++) {
                    getOwnerNoeudRequest(i, 'r');
            }
        }
    }
    if (msg->getArrivalGate()) {
        cGate *g = msg->getArrivalGate();
        int pass = g->getIndex();
        //a la reception d'un message informatif d'un noeud propreitaire
        if (dynamic_cast<ACK*>(msg) != nullptr) {
            ACK *msg_r = check_and_cast<ACK*>(msg);
            if (msg_r->getStatus()) {
                EV << "Noeud propreitaire de l'indice " << msg_r->getTabIndice()
                          << " est: " << msg_r->getNoeudOwner() << endl;
                //envoyer une reponse de lecture aupres du noeud propreitaire
                if (msg_r->getMethode() == 'r') {
                    readRequest(msg_r->getNoeudOwner(), msg_r->getTabIndice());
                } else {
                    writeRequest(msg_r->getNoeudOwner(), msg_r->getTabIndice(),
                            par("somme").intValue());
                }
            } else {
                EV << "L'indice [" << msg_r->getTabIndice()
                          << "] n'a pas un noeud propreitaire" << endl;
            }
        }

        if (dynamic_cast<readMessage*>(msg) != nullptr) {
            readMessage *msg_r = check_and_cast<readMessage*>(msg);
            EV << "La valeur demandée ayant l'indice [" << msg_r->getTabIndice()
                      << "] est: "
                      << stringToIntVector(par("localTab"), ' ')[msg_r->getTabIndice()]
                      << endl;
            //envoyer une reponse au noeud
            readResponse(msg_r->getTabIndice(), pass);
        }
        if (dynamic_cast<writeMessage*>(msg) != nullptr) {
            writeMessage *msg_r = check_and_cast<writeMessage*>(msg);
            EV << "Demande d'ecriture de la valeur ayant l'indice ["
                      << msg_r->getTabIndice() << "] de=: "
                      << stringToIntVector(par("localTab"), ' ')[msg_r->getTabIndice()]
                      << "à =:" << msg_r->getNewVal() << endl;
            // mise a jour
            if (update(msg_r->getTabIndice(), msg_r->getNewVal())) {
                // mise a jour reussie
                writeResponse(msg_r->getTabIndice(), pass);
            }
        }
        if (dynamic_cast<rwACK*>(msg) != nullptr) {
            rwACK *msg_r = check_and_cast<rwACK*>(msg);
            if (msg_r->getStatus()) {
                // NB:: Deux scenarios, si on veut mis a jour la table locale avec la valeur de l'indice demandee,
                // on fait update, sinon on enleve la ligne 95
//                update(msg_r->getTabIndice(),msg_r->getValue());
                if (msg_r->getType() == 'r') {
                    int remaining = par("reponsesRestees").intValue() - 1;
                                    par("reponsesRestees").setIntValue(remaining);
                    EV << "J'ai recu une reponse du noeud pour l'indice ["
                              << msg_r->getTabIndice() << "] =: "
                              << msg_r->getValue() << endl;
                    if (msg_r->getType() == 'r') {
                        par("somme").setIntValue(
                                par("somme").intValue() + msg_r->getValue());
                        if (getIndex() == 3 && remaining == 0) {
                            checkAndUpdateFinal(19);
                        } else if (getIndex() == 1 && remaining == 0) {
                            checkAndUpdateFinal(0);
                        }
                    }
                } else {
                    EV<< "J'ai recu un ack reussi du noeud propeitaire pour l'ecriture sur l'indice ["
                              << msg_r->getTabIndice()
                              << "] avec la nouvelle valeur "
                              << msg_r->getValue() << endl;
                }
            } else {
                EV << "Impossible de lire/Ecrire a cet indice ["
                          << msg_r->getTabIndice() << "]" << endl;
            }
        }

    }
}

Define_Module(Noeud);
