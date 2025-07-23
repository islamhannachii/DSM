//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef NOEUD_H_
#define NOEUD_H_
#include <omnetpp.h>
#include <string.h>
#include <mutex>
#include <ACK_m.h>
#include <getNodeOwner_m.h>
#include <readMessage_m.h>
#include <writeMessage_m.h>
#include <rwACK_m.h>
using namespace omnetpp;

class Noeud: public cSimpleModule {
protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override {
        EV << " Ma table locale: " << par("localTab").stringValue() << endl;
        if (par("somme").intValue())
            EV << "Noeud " << getIndex() << " la somme est: "
                      << par("somme").intValue() << endl;
    }
    virtual void getOwnerNoeudRequest(int tabIndice, char methode) {
        getNodeOwner *m = new getNodeOwner("getNodeOwner");
        m->setTabIndice(tabIndice);
        m->setMethode(methode);
//        sendDelayed(m, exponential(1.0), "c$o", 0);
        send(m, "c$o", 0);
    }
    virtual void readRequest(int noeudCible, int tabIndice) {
        readMessage *r = new readMessage("ReadRequest");
        r->setSource(getIndex());
        r->setTabIndice(tabIndice);
        int numGates = gateSize("c$o");
        for (int i = 0; i < numGates; ++i) {
            cGate *outGate = gate("c$o", i);
            cGate *inGate = outGate->getNextGate();
            if (inGate && inGate->getOwnerModule()) {
                cModule *ownerModule = inGate->getOwnerModule();
                if ((ownerModule->isVector())
                        && (ownerModule->getIndex() == noeudCible)) {
//                    sendDelayed(r, exponential(1.0), "c$o", i);
                    send(r, "c$o", i);
                }
            }
        }
    }
    virtual void readResponse(int tabIndice, int pass) {
        int valeur = stringToIntVector(par("localTab"), ' ')[tabIndice];
        if (valeur != -1) {
            rwACK *a = new rwACK("readResponse");
            a->setStatus(true);
            a->setTabIndice(tabIndice);
            a->setNoeudOwner(getIndex());
            a->setValue(valeur);
            a->setType('r');
//            sendDelayed(a, exponential(1.0), "c$o", pass);
            send(a, "c$o", pass);
        } else {
            rwACK *a = new rwACK("readResponse");
            a->setStatus(false);
            a->setTabIndice(tabIndice);
            a->setNoeudOwner(getIndex());
            a->setValue(-1);
            a->setType('r');
            send(a, "c$o", pass);
        }
    }
    virtual void writeResponse(int tabIndice, int pass) {
        int valeur = stringToIntVector(par("localTab"), ' ')[tabIndice];
        if (valeur != 0) {
            rwACK *a = new rwACK("writeResponse");
            a->setStatus(true);
            a->setTabIndice(tabIndice);
            a->setNoeudOwner(getIndex());
            a->setValue(valeur);
            a->setType('w');
//            sendDelayed(a, exponential(1.0), "c$o", pass);
            send(a, "c$o", pass);
        } else {
            rwACK *a = new rwACK("writeResponse");
            a->setStatus(false);
            a->setTabIndice(tabIndice);
            a->setNoeudOwner(getIndex());
            a->setValue(-1);
            a->setType('w');
            send(a, "c$o", pass);
        }
    }
    virtual void writeRequest(int noeudCible, int tabIndice, int val) {
        writeMessage *r = new writeMessage("writeRequest");
        r->setSource(getIndex());
        r->setTabIndice(tabIndice);
        r->setNewVal(val);
        int numGates = gateSize("c$o");
        for (int i = 0; i < numGates; ++i) {
            cGate *outGate = gate("c$o", i);
            cGate *inGate = outGate->getNextGate();
            if (inGate && inGate->getOwnerModule()) {
                cModule *ownerModule = inGate->getOwnerModule();
                if (ownerModule->isVector()
                        && ownerModule->getIndex() == noeudCible) {
//                    sendDelayed(r, exponential(1.0), "c$o", i);
                    send(r, "c$o", i);

                }
            }
        }
    }
    virtual void checkAndUpdateFinal(int updateIndex) {
        if (par("reponsesRestees").intValue() == 0) {
            getOwnerNoeudRequest(updateIndex, 'w');
        }
    }
    virtual bool update(int tabIndice, int newVal) {
        try {
            std::vector<int> nowLocalTab = stringToIntVector(par("localTab"),
                    ' ');
            nowLocalTab[tabIndice] = newVal;
            par("localTab").setStringValue(intVectorToString(nowLocalTab, ' '));
            return true;
        } catch (cException ex) {
            EV << "Erreur lors de la mise a jour!!" << endl;
            return false;
        }
    }
    virtual std::string remplirLocalTab(std::string localTab, int noeudId,
            int minVal = 0, int maxVal = 100) {
        std::vector<int> result = stringToIntVector(localTab, ' ');
        switch (noeudId) {
        case 0:
            for (int i = 0; i < 4; i++) {
                result[i] = intuniform(minVal, maxVal);
            }
            break;
        case 1:
            for (int i = 4; i < 8; i++) {
                result[i] = intuniform(minVal, maxVal);
            }
            break;
        case 2:
            for (int i = 8; i < 12; i++) {
                result[i] = intuniform(minVal, maxVal);
            }
            break;
        case 3:
            for (int i = 12; i < 16; i++) {
                result[i] = intuniform(minVal, maxVal);
            }
            break;
        case 4:
            for (int i = 16; i < 20; i++) {
                result[i] = intuniform(minVal, maxVal);
            }
            break;
        default:
            break;
        }
        return intVectorToString(result, ' ');
    }
    virtual std::vector<int> stringToIntVector(const std::string &str,
            char separator) {
        std::vector<int> result;
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, separator)) {
            result.push_back(std::stoi(token));
        }

        return result;
    }
    virtual std::string intVectorToString(const std::vector<int> &vec,
            char separator) {
        std::ostringstream oss;

        for (size_t i = 0; i < vec.size(); ++i) {
            oss << vec[i]; //
            if (i < vec.size() - 1) {
                oss << separator;
            }
        }
        return oss.str();
    }
};

#endif /* NOEUD_H_ */
