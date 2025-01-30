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
void updateISvektor(std::vector<int>& IS, std::vector<int>& RN,  std::vector<std::tuple<int, int>>& ES, std::vector<int>& ISD, std::vector<int>& VC){
    // Funktion zum updaten des Independent Set Vektors für den Neighbor Cover Algorithmus
    // IS: Independent Set vektor;  RN: Vektor der Priorität der Knoten RN[v] := Position des Knoten v in Prio Liste; 
    // ES: Kantenstream , ISD: Vektor für dyn. Graph Streams, zählt Anzahl der Kanten zu Knoten mit niedrigerer Prio; 
    // VC: Vertex Cover Vektor
    for (int i = 0; i < ES.size(); i++){
        // Jede Kante des Graphstreams durchgehen und den ISD Vektor entsprechend aktualisieren
        int u = std::get<0>(ES[i]);
        int v = std::get<1>(ES[i]);
 
        if (IS[v] == 0 && IS[u] == 0 && VC[v] == 0 && VC[u] == 0) {
            // die Kante wird nur berücksichtigt, wenn noch keiner der Knoten in IS oder VC ist
            if (RN[v] < RN[u]){
                ISD[u] = 1; 
            } 
            else {
                ISD[v] = 1;
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

void updateVCvektor(std::vector<int>& IS, std::vector<int>& VC, std::vector<tuple<int,int>>& ES){
    // Funktion zum updaten des Vertex Cover Vektors für Neighbor Cover
    // IS: Independent Set Vektor, VC: Vertex Cover Vektor, ES Kantenstream
    for (int i = 0; i < ES.size(); i++){
        // Jede Kante des Graphstreams durchgehen und den VC Vektor entsprechend aktualisieren
        int u = std::get<0>(ES[i]);
        int v = std::get<1>(ES[i]);

        // Konten zu VC hinzufügen, wenn er einen Nachbarn im Independent Set hat
        if (IS[u] == 0 && IS[v] == 1) {
            VC[u] = 1;
        } else if (IS[v] == 0 && IS[u] == 1) {
            VC[v] = 1;
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

std::pair<int, vector<int>> NeighborCover(int numNodes, std::vector<std::tuple<int, int>>& Edges){
    // Funktion zum Anwenden des Neighbor Cover Algorithmus auf einen Graphen
    std::vector<int> VC(numNodes, 0);  // VC-Vector mit der Groesse der generierten Knotenmenge, alle auf 0 initialisiert
    std::vector<int> IS(numNodes, 0);  // IS-Vector mit der Groesse der generierten Knotenmenge, alle auf 0 initialisiert
    std::vector<int> ISD(numNodes, 0); // Vektor um ausgehende Kanten zu markieren
    bool allin = false; // Variable zum Überprüfen, ob alle Knoten in VC oder IS
    
    // Erstellen der zufälligen Knotenreihenfolge
    // Vektor zum Speichern der zufälligen Knotenreihenfolge
    std::vector<int> randomNodes;
    
    // Alle Knoten in Vektor einfügen
    for (int i = 1; i <= numNodes; ++i) {
        randomNodes.push_back(i);
    }
    // Zufallsgenerator mit Startwert initialisieren
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

    int count = 0; // Counter für Anzahl an Stream-Durchläufen
    while (!allin){
        updateISvektor(IS, randomNodes, Edges, ISD, VC);
        updateVCvektor(IS, VC, Edges);
        count += 2; //(1 Durchlauf für IS, einer für VC)
        allin = allOnes(IS,VC);
        
        for (int i = 0; i < ISD.size(); ++i) {
             ISD[i] = 0;
        }
        

    }
    
    return std::make_pair(count, VC);

}

std::vector<int>  GreedyVC(int numNodes, std::vector<std::pair<int,int>>& Edges){
    // Greedy für Vertex Cover (für ungewichtete Graphen)
    std::vector<int> GVC(numNodes, 0); //Vektor für Vertex Cover
    for( int i = 0; i < Edges.size(); i++){
        int u  = Edges[i].first;
        int v  = Edges[i].second;
        if (GVC[u] == 0 && GVC[v] == 0){
            GVC[u] = 1;
            GVC[v] = 1;
        }
    }
    return GVC;
}


struct GraphData {
    int n = 0;
    int m = 0;
    double p = -1.0;
    int d = -1;
    std::vector<std::pair<int, int>> edges; // Ändere hier auf tuple
};


// Funktion zum Parsen der Graph-Informationen
GraphData parseGraphFile(const std::string& graphFilePath) {
    GraphData graphData;
    std::ifstream graphFile(graphFilePath);

    if (!graphFile.is_open()) {
        throw std::runtime_error("Fehler beim Öffnen der Datei: " + graphFilePath);
    }

    std::string line;
    
    if (std::getline(graphFile, line)) {
        // Extrahiere n und den spezifischen Parameter
        if (line.find("n=") != std::string::npos) {
            std::istringstream iss(line);
            std::string nStr, paramStr;
            std::getline(iss, nStr, ',');
            std::getline(iss, paramStr, ',');
            graphData.n = std::stoi(nStr.substr(3));  // Entferne "%n="

            if (paramStr.find("m=") != std::string::npos) {
                graphData.m = std::stoi(paramStr.substr(2));  // "m=..."
            } else if (paramStr.find("p=") != std::string::npos) {
                graphData.p = std::stod(paramStr.substr(2));  // "p=..."
            } else if (paramStr.find("d=") != std::string::npos) {
                graphData.d = std::stoi(paramStr.substr(2));  // "d=..."
            }
        }
    }

    while (std::getline(graphFile, line)) {
        std::istringstream edgeStream(line);
        int u, v;
        if (edgeStream >> u && edgeStream.ignore() && edgeStream >> v) {
            graphData.edges.emplace_back(u, v);
        }
    }

    graphFile.close();
    graphData.m = static_cast<int>(graphData.edges.size());
    return graphData;
}

// Ergebnisdateien für verschiedene Graph-Typen erstellen
std::string getOutputFilePath(const std::string& outputDir, const std::string& graphFilePath) {
    if (graphFilePath.find("gnm_random") != std::string::npos) {
        return outputDir + "/gnm_random_results_greedy.csv";
    } else if (graphFilePath.find("gnp_random") != std::string::npos) {
        return outputDir + "/gnp_random_results_greedy.csv";
    } else if (graphFilePath.find("rrg") != std::string::npos) {
        return outputDir + "/rrg_results_greedy.csv";
    } else if (graphFilePath.find("gnm_random_weight") != std::string::npos) {
        return outputDir + "/gnm_random_weight_results_greedy.csv";
    } else {
        return outputDir + "/BarabasiAlbert_neu_results_greedy.csv";
    }
    throw std::runtime_error("Unbekannter Graph-Typ im Pfad: " + graphFilePath);
}

// Verarbeitung der Graphen und Speicherung der Ergebnisse
void processGraphs(const std::string& inputCsvFilePath, const std::string& outputDir) {
    std::ifstream inputCsv(inputCsvFilePath);
    if (!inputCsv.is_open()) {
        throw std::runtime_error("Fehler beim Öffnen der Eingabedatei: " + inputCsvFilePath);
    }

    std::string line;
    std::getline(inputCsv, line); // Überspringe Headerzeile
    while (std::getline(inputCsv, line)) {
        std::istringstream lineStream(line);
        std::string graphFilePath;
        std::getline(lineStream, graphFilePath, ',');

        GraphData graphData = parseGraphFile(graphFilePath);

        // GreedyVc ausführen
        std::vector<int> vcVector = GreedyVC(graphData.n, graphData.edges);

        // Speicherpfad bestimmen
        std::string outputFilePath = getOutputFilePath(outputDir, graphFilePath);

        // Ergebnis in CSV-Datei speichern
        std::ofstream outputCsv(outputFilePath, std::ios::app);
        if (!outputCsv.is_open()) {
            throw std::runtime_error("Fehler beim Öffnen der Ausgabedatei: " + outputFilePath);
        }

        // Graphdaten und VC-Ergebnis in Datei speichern
        outputCsv << graphFilePath << "," << graphData.n << "," << graphData.m << ",";

        if (graphData.p != -1.0) {
            outputCsv << "," << graphData.p;
        } else if (graphData.d != -1) {
            outputCsv << "," << graphData.d;
        }

        int sizeVC = 0;

        for (int i = 0; i < vcVector.size(); i++){
            if ( vcVector[i] == 1){
                sizeVC += 1;
            }
        }

        outputCsv <<  sizeVC << ",";
        // VC-Vektor speichern mit Leerzeichen als Trennzeichen
        for (size_t i = 0; i < vcVector.size(); ++i) {
            if (i > 0) outputCsv << " ";  // Leerzeichen für Trennung
            outputCsv << vcVector[i];
        }
        outputCsv << "\n";  // Neue Zeile für den nächsten Eintrag
    }
}

// Hauptprogramm
int main() {
    std::string inputCsvFilePath = "C://graph_info.csv"; // hier Pfad zur Graph-Info CSV eingeben
    std::string outputDir = "C://results"; //hier Pfad angeben, indem Ergebnissdatei gespeichert werden soll

    if (!std::filesystem::exists(outputDir)) {
        std::filesystem::create_directories(outputDir);
        std::cout << "Ausgabeverzeichnis erstellt: " << outputDir << "\n";
    }

    try {
        processGraphs(inputCsvFilePath, outputDir);
        std::cout << "Die Verarbeitung der Graphen und das Speichern der Ergebnisse wurde abgeschlossen.\n";
    } catch (const std::exception& e) {
        std::cerr << "Fehler bei der Verarbeitung: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
//  int main(){

//     // Variablen zum Speichern Knoten- und Kantenanzahl
//     int numNodes = 0, numEdges = 0, numOperations = 0;
//     // Kantenstream
//     std::vector<std::tuple<int, int>> Edges;

//     for( int i = 0 ; i<100; i++){
//         std::pair<int, std::vector<int>> ergebnis = NeighborCover(numNodes, Edges, node_weights);
//     }

//     std::vector<int> VCG = GreedyVC(numNodes, Edges);

// }