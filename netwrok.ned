simple Server
{
    gates:
        inout p[];
}
simple Noeud
{
    parameters:
        string localTab @mutable;
        double d;
        int somme @mutable = 0;
        int reponsesRestees @mutable = 0;
    gates:
        inout c[];
}
network Reseau
{
    submodules:
        s: Server {
            @display("i=device/server");

        }
        noeud[5]: Noeud {
            @display("i=device/pc");
        }
    connections:
        s.p++ <--> noeud[0].c++;
        s.p++ <--> noeud[1].c++;
        s.p++ <--> noeud[2].c++;
        s.p++ <--> noeud[3].c++;
        s.p++ <--> noeud[4].c++;
        noeud[0].c++ <--> noeud[1].c++;
        noeud[0].c++ <--> noeud[2].c++;
        noeud[0].c++ <--> noeud[3].c++;
        noeud[0].c++ <--> noeud[4].c++;
        noeud[1].c++ <--> noeud[2].c++;
        noeud[1].c++ <--> noeud[3].c++;
        noeud[1].c++ <--> noeud[4].c++;
        noeud[2].c++ <--> noeud[3].c++;
        noeud[2].c++ <--> noeud[4].c++;
        noeud[3].c++ <--> noeud[4].c++;

}
