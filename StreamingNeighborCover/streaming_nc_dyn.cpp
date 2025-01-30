#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <utility>
#include <ctime>
#include <algorithm>
#include <random>
#include <chrono>
#include <bits/stdc++.h> 



using namespace std;

// Funktionen für den Neighbor Cover Algorithmus
void updateISvektor(std::vector<int>& IS, std::vector<int>& RN,  std::vector<std::tuple<int, int, int>>& ES, std::vector<int>& ISD, std::vector<int>& VC){
    // Funktion zum updaten des Independent Set Vektors für den Neighbor Cover Algorithmus
    // IS: Independent Set vektor;  RN: Vektor der Priorität der Knoten RN[v] := Position des Knoten v in Prio Liste; 
    // ES: Kantenstream , ISD: Vektor für dyn. Graph Streams, zählt Anzahl der Kanten zu Knoten mit niedrigerer Prio; 
    // VC: Vertex Cover Vektor
    for (int i = 0; i < ES.size(); i++){
        // Jede Kante des Graphstreams durchgehen und den ISD Vektor entsprechend aktualisieren
        int u = std::get<0>(ES[i]);
        int v = std::get<1>(ES[i]);
        int dyn = std::get<2>(ES[i]);

        if (IS[v] == 0 && IS[u] == 0 && VC[v] == 0 && VC[u] == 0) {
            // die Kante wird nur berücksichtigt, wenn noch keiner der Knoten in IS oder VC ist
            if (RN[v] < RN[u]){
                ISD[u] += dyn; 
            } 
            else {
                ISD[v] += dyn;
            }
        }
    }

    // Nun den IS Vektor updaten: 
    for (int i = 0; i < ISD.size(); ++i) {
        // wenn ein Knoten unter den betrachteten Kanten keine inzidente zu einem Knoten mit schlechterer Priorität hat, wird er zu IS hinzugefügt
        if (ISD[i] == 0){
            IS[i] = 1;
        }
    }
}

void updateVCvektor(std::vector<int>& IS, std::vector<int>& VC, std::vector<tuple<int,int,int>>& ES, std::vector<int>& VCD){
    // Funktion zum updaten des Vertex Cover Vektors für Neighbor Cover
    // IS: Independent Set Vektor, VC: Vertex Cover Vektor, ES Kantenstream
    for (int i = 0; i < ES.size(); i++){
        // Jede Kante des Graphstreams durchgehen und den VC Vektor entsprechend aktualisieren
        int u = std::get<0>(ES[i]);
        int v = std::get<1>(ES[i]);
        int d = std::get<2>(ES[i]);

        // Konten zu VC hinzufügen, wenn er einen Nachbarn im Independent Set hat
        if (IS[u] == 0 && IS[v] == 1) {
            VCD[u] += d ;
        } else if (IS[v] == 0 && IS[u] == 1) {
            VCD[v] += d;
        }   
        
        // Nun den VC Vektor updaten: 
        for (int i = 0; i < VCD.size(); ++i) { 
            if (VCD[i] > 0){
                VC[i] = 1;
            }
        }
        
    }
}


bool allOnes(std::vector<int> IS, std::vector<int> VC){
    // Überprüfe ob alle Knoten in Is oder VC
    for (int i = 0; i < IS.size(); i++){
        if (IS[i] == 0 && VC[i] == 0){
            return false;
        }
    }
    return true;
}

std::pair<int, vector<int>> NeighborCover(int numNodes, std::vector<std::tuple<int, int, int>>& Edges){
    // Funktion zum Anwenden des Neighbor Cover Algorithmus auf einen Graphen
    std::vector<int> VC(numNodes, 0);  // VC-Vector mit der Groesse der generierten Knotenmenge, alle auf 0 initialisiert
    std::vector<int> IS(numNodes, 0);  // IS-Vector mit der Groesse der generierten Knotenmenge, alle auf 0 initialisiert
    std::vector<int> ISD(numNodes, 0); // Vektor um ausgehende Kanten zu zählen
    std::vector<int> VCD(numNodes, 0); // Vektor um ausgehende Kanten zu zählen
    bool allin = false; // Variable zum Überprüfen, ob alle Knoten in VC oder IS
    
    // Erstellen der zufälligen Knotenreihenfolge

    //std::vector<int> randomNodes = weightedShuffle(node_weights);
     // Vektor zum Speichern der zufälligen Knotenreihenfolge
    std::vector<int> randomNodes;
    
    // Alle Knoten in Vektor einfügen
    for (int i = 1; i <= numNodes; ++i) {
        randomNodes.push_back(i);
    }
    // Zufallsgenerator mit Startwert initialisieren
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(randomNodes.begin(), randomNodes.end(), std::default_random_engine(seed));

    int count = 0; // Counter für Anzahl an Stream-Durchläufen
    while (!allin){
        updateISvektor(IS, randomNodes, Edges, ISD, VC);
        updateVCvektor(IS, VC, Edges, VCD);
        count += 2; //(1 Durchlauf für IS, einer für VC)
        allin = allOnes(IS,VC);

        for (int i = 0; i < ISD.size(); ++i) {
             ISD[i] = 0;
             VCD[i] = 0;
        }
        

    }
    
    return std::make_pair(count, VC);

}

struct GraphData {
    int n = 0;
    int m = 0;
    double p = -1.0;
    int dd = -1;
    std::vector<std::tuple<int, int, int>> edges; // Kanten als Tripel (u, v, d)
};

// Funktion zum Einlesen der Graph-Daten aus einer .txt Datei
GraphData parseGraphFile(const std::string& filePath) {
    std::ifstream file(filePath);
    GraphData graphData;
    std::string line;

    // Lese die erste Zeile und extrahiere n und den spezifischen Parameter (m, p oder d)
    if (std::getline(file, line)) {
        if (line.find(",m=") != std::string::npos) {
            std::sscanf(line.c_str(), "%%n=%d,m=%d", &graphData.n, &graphData.m);
        } else if (line.find(",p=") != std::string::npos) {
            char paramStr[50];
            std::sscanf(line.c_str(), "%%n=%d,p=%s", &graphData.n, paramStr);
            graphData.p = std::stod(paramStr);
        } else if (line.find(",d=") != std::string::npos) {
            std::sscanf(line.c_str(), "%%n=%d,d=%d", &graphData.n, &graphData.dd);
        }
    }

    // Lese die Kanten (u, v, d) ab der zweiten Zeile
    int u, v, d;
    while (file >> u) {
        if (file.peek() == ',') {
            file.ignore();  // Ignoriere das Komma
            file >> v;
            if (file.peek() == ',') {
                file.ignore();  // Ignoriere das Komma
                file >> d;
                graphData.edges.emplace_back(u, v, d); // Speichere die Kante als Tripel
            }
        }
    }

    // Falls m nicht gesetzt ist, berechne es als die Anzahl der Kanten
    if (graphData.m == 0) {
        graphData.m = graphData.edges.size();
    }

    return graphData;
}

// Hauptfunktion zum Verarbeiten der Graphen und Speichern der Ergebnisse in CSV-Dateien
void processGraphs(const std::string& csvFilePath, const std::string& outputDir) {
    std::ifstream csvFile(csvFilePath);
    std::string line;
    std::getline(csvFile, line); // Überspringe die Headerzeile

    while (std::getline(csvFile, line)) {
        std::stringstream ss(line);
        std::string graphFilePath;
        int n;
        std::getline(ss, graphFilePath, ',');
        ss >> n;

        // Lese die Graphdaten, einschließlich Parameter und Kanten
        GraphData graphData = parseGraphFile(graphFilePath);

        // Bestimme die Ausgabe-Datei basierend auf dem Graph-Typ
        std::string outputFile;
        if (graphFilePath.find("gnm_random") != std::string::npos) {
            outputFile = outputDir + "/dyn_gnm_random_results.csv";
        } else if (graphFilePath.find("gnp_random") != std::string::npos) {
            outputFile = outputDir + "/dyn_gnp_random_results.csv";
        } else if (graphFilePath.find("rrg") != std::string::npos) {
            outputFile = outputDir + "/dyn_rrg_results.csv";
        } else if (graphFilePath.find("gnm_random_weight") != std::string::npos) {
            outputFile = outputDir + "/dyn_gnm_random_weight_results.csv";
        }

        std::ofstream outFile(outputFile, std::ios::app);

        // Führt 100 Wiederholungen für den Algorithmus aus und speichert die Ergebnisse
        for (int i = 0; i < 100; ++i) {
            auto [count, VC] = NeighborCover(graphData.n, graphData.edges);

            // Schreibe die Ergebnisse in die CSV-Datei
            outFile << graphFilePath << "," << graphData.n << "," << graphData.m << ",";

            // Spezifische Parameter in der CSV speichern
            if (graphData.p >= 0) {
                outFile << "p=" << graphData.p << ",";
            } else if (graphData.dd >= 0) {
                outFile << "d=" << graphData.dd << ",";
            }

            outFile << count << ",";
            int sizeVC = 0;

            for (int i = 0; i < VC.size(); i++) {
                if (VC[i] == 1) {
                    sizeVC += 1;
                }
            }

            outFile << sizeVC << ",";
            for (size_t j = 0; j < VC.size(); ++j) {
                outFile << VC[j] << (j < VC.size() - 1 ? " " : "");
            }
            outFile << "\n";
        }
    }
}

int main() {
    // Pfad zur Eingabe-CSV-Datei mit den Informationen zu den Graphen
    std::string inputCsvFilePath = "";

    // Verzeichnis für die Ausgabe-CSV-Dateien
    std::string outputDir = "";
    std::cout << "START" << std::endl;
    // Verarbeite die Graphen und speichere die Ergebnisse
    try {
        processGraphs(inputCsvFilePath, outputDir);
        std::cout << "Die Verarbeitung der Graphen und das Speichern der Ergebnisse wurde abgeschlossen.\n";
    } catch (const std::exception& e) {
        std::cerr << "Fehler bei der Verarbeitung: " << e.what() << "\n";
        return 1;
    }

    return 0;
}