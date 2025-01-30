import os
import csv
import random

# Eingabe- und Ausgabepfade
csv_input_file = r"C:\Users\Chantal\network-repository\networks.csv"
csv_output_file = r"C:\Users\Chantal\network-repository\network-repository-dyn_bis_ht.csv"
base_path = r"C:\Users\Chantal\network-repository"

def process_graph_file(graph_path, n, m, operations_factors):
    """
    Erstellt dynamische Graphstreams mit präziser Anzahl an Operationen.
    Speichert die Streams in Dateien und gibt ihre Pfade und Eigenschaften zurück.
    """
    # Einlesen der originalen Kanten
    edges = []
    with open(graph_path, 'r') as f:
        lines = f.readlines()
        for line in lines[1:]:  # Erste Zeile überspringen
            u, v = map(int, line.strip().split(','))
            edges.append((u, v))
    
    # Erstelle die vollständige Menge aller möglichen Kanten für den Graphen
    all_edges = set((i, j) for i in range(n) for j in range(n) if i != j)

    # Setze den Status jeder Kante auf 'none' (d.h. noch nicht hinzugefügt oder gelöscht)
    edge_status = {edge: 'none' for edge in all_edges}
    
    generated_files = []

    for factor in operations_factors:
        num_operations = int(round(factor * m))  # Anzahl der Operationen berechnen
        stream_operations = []
        
        # 1. Führe zufällige Kantenoperationen durch, bevor echte Kanten hinzugefügt werden
        added_operations = 0
        while added_operations < num_operations // 3:
            u, v = random.randint(0, n-1), random.randint(0, n-1)
            if u == v:
                continue  # Keine Schleifen erlauben
            
            if edge_status[(u, v)] == 'none':
                # Hinzufügen der Kante
                stream_operations.append((u, v, 1))
                edge_status[(u, v)] = 'added'
            elif edge_status[(u, v)] == 'added':
                # Löschen der Kante
                stream_operations.append((u, v, -1))
                edge_status[(u, v)] = 'deleted'
            # Wenn die Kante bereits gelöscht wurde, überspringe sie
            added_operations += 1
        
        # 2. Füge alle echten Kanten hinzu in zufälliger Reihenfolge
        random.shuffle(edges)  # Kanten zufällig mischen
        for u, v in edges:
            if edge_status[(u, v)] == 'none':
                stream_operations.append((u, v, 1))
                edge_status[(u, v)] = 'added'
        
        # 3. Generiere zusätzliche Operationen, um die Gesamtanzahl zu erreichen
        added_operations = len(edges)
        while added_operations < num_operations:
            u, v = random.randint(0, n-1), random.randint(0, n-1)
            if u == v:
                continue  # Keine Schleifen erlauben
            
            if edge_status[(u, v)] == 'none':
                # Hinzufügen der Kante
                stream_operations.append((u, v, 1))
                edge_status[(u, v)] = 'added'
            elif edge_status[(u, v)] == 'added':
                # Löschen der Kante
                stream_operations.append((u, v, -1))
                edge_status[(u, v)] = 'deleted'
            
            added_operations += 1
        
        # 4. Speichere die dynamische Datei mit entsprechendem Suffix und .txt-Endung
        base_name = os.path.splitext(os.path.basename(graph_path))[0]  # Entfernt die Endung .simp-undir-edges
        dynamic_file_path = os.path.join(os.path.dirname(graph_path), f"{base_name}_dyn_{factor:.1f}.txt")
        
        with open(dynamic_file_path, 'w') as f:
            f.write(f"%n={n},m={m},p={len(stream_operations)}\n")
            for u, v, d in stream_operations:
                f.write(f"{u},{v},{d}\n")
        
        generated_files.append((dynamic_file_path, n, m, len(stream_operations)))
    
    return generated_files

def main():
    operations_factors = [1.1, 1.2, 1.5]  # Exakte Faktoren
    output_rows = []

    # Einlesen der Eingabe-CSV
    with open(csv_input_file, 'r') as infile:
        reader = csv.reader(infile)
        next(reader)  # Header überspringen
        for row in reader:
            graphfile, n, m = row[0], int(row[1]), int(row[2])
            graph_path = os.path.join(base_path, graphfile)
            
            if not os.path.exists(graph_path):
                print(f"Datei {graph_path} nicht gefunden, überspringe...")
                continue
            
            if n >= 1000:
                print(f"Graph {graphfile} hat zu viele Knoten ({n}), überspringe...")
                continue

            # Verarbeite die Graphdatei und generiere dynamische Streams
            print(f"Verarbeite {graphfile}...")
            dynamic_files = process_graph_file(graph_path, n, m, operations_factors)
            for dyn_file, n, m, p in dynamic_files:
                output_rows.append([dyn_file, n, m, p])

    # Schreibe die Ergebnis-CSV-Datei
    with open(csv_output_file, 'w', newline='') as outfile:
        writer = csv.writer(outfile)
        writer.writerow(["graphfile", "nodes", "edges", "operations"])  # Header
        writer.writerows(output_rows)

    print(f"Verarbeitung abgeschlossen. Ergebnis-CSV: {csv_output_file}")

if __name__ == "__main__":
    main()
