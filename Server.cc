#include "Server.h"
#include <omnetpp.h>

Define_Module(Server);

void Server::initialize() {
    EV << "Serveur: " << endl;
    // initialiser l'annuaire

    initialiserAnnuaire();
    EV << "Indice\tNoeud Propreitaire"<<endl;
    EV << "------------------------\n";
    for (item i : annuaireTab) {
        EV << i.indice << "\t | \t" << i.noeudPropreitaire <<endl;;
    }
}

void Server::handleMessage(cMessage *msg) {

    cGate *g = msg->getArrivalGate();
    int pass = g->getIndex();
    // A la reception de demande d'Id de noeud propreitaire
    if (dynamic_cast<getNodeOwner*>(msg) != nullptr) {
        getNodeOwner *msg_r = check_and_cast<getNodeOwner*>(msg);
        int nodeOwner = annuaireTab[(int) (msg_r->getTabIndice())].noeudPropreitaire;
        if (nodeOwner != -1) {
            EV << "Annuaire[" << msg_r->getTabIndice() << "] =:" << nodeOwner
                      << endl;
            //Envoyer une reponse ACK avec status egale a succes et l'id du noeud propreitaire
            ACK *reponse = new ACK("ACK");
            reponse->setStatus(true);
            reponse->setNoeudOwner(nodeOwner);
            reponse->setTabIndice(msg_r->getTabIndice());
            reponse->setMethode(msg_r->getMethode());
            sendDelayed(reponse, exponential(1.0),"p$o", pass);

        } else {
            ACK *reponse = new ACK("ACK");
            reponse->setStatus(false);
            reponse->setNoeudOwner(-1);
            reponse->setTabIndice(msg_r->getTabIndice());
            send(reponse, "p$o", pass);
        }
    }
}
