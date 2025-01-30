# -*- coding: utf-8 -*-
"""
Created on Sat Oct 26 13:01:12 2024

@author: Chantal
"""
import networkit as nk
import random
import os
import csv

bag = nk.generators.BarabasiAlbertGenerator(3, 10)
bagG = bag.generate()

for u,v in bagG.iterEdges():
    print(u, v)
    
    
    
hg = nk.generators.HyperbolicGenerator(5000,16,7)
hgG = hg.generate()

for u,v in hgG.iterEdges():
    print(u,v)
    


# Funktion, um die höchste vorhandene Dateinummer im Verzeichnis zu finden
def get_highest_file_number(directory, prefix, suffix):
    if not os.path.exists(directory):
        return 0
    files = [f for f in os.listdir(directory) if f.startswith(prefix) and f.endswith(suffix)]
    numbers = [int(f[len(prefix):-len(suffix)]) for f in files if f[len(prefix):-len(suffix)].isdigit()]
    return max(numbers, default=0)

    
for x in [1,2,3,5,10,15,20,40,50,100,200,500,700]:
    num_graphs = 10
    nMax = 1000
   
    
    
    output_dir = 'C:\\Users\\Chantal\\Desktop\\Uni\\Masterarbeit\\networkit\\BarabasiAlbert_neu\\nMax'+str(nMax)+'_k'+str(x)
    file_prefix = 'graph_'
    file_suffix = '.txt'
    node_weights_file_prefix = 'node_weights_'
    csv_dir = 'C:\\Users\\Chantal\\Desktop\\Uni\\Masterarbeit\\networkit'
    os.makedirs(output_dir, exist_ok=True)
    start_num = get_highest_file_number(output_dir, file_prefix, file_suffix) + 1
    
    for j in range(num_graphs):
        bag = nk.generators.BarabasiAlbertGenerator(x, nMax)
        G = bag.generate()
        n = G.numberOfNodes()
        m = G.numberOfEdges()

        #for node in G.iterNodes():
         #  G.nodes[node]['weight'] = random.randint(1, n)  # zufälliges Gewicht zwischen 1 und n
       
        
        file_num = start_num +j
        graph_filename = os.path.join(output_dir, f"{file_prefix}{file_num}{file_suffix}")
        for k in range(10):
            filename = os.path.join(output_dir, f"{file_prefix}{file_num}_{k}{file_suffix}")
            edges = list(G.iterEdges())
            random.shuffle(edges)
               
            with open(filename, 'w') as file:
                file.write(f"%n={n},k={x}\n")
                for edge in edges:
                    file.write(f"{edge[0]},{edge[1]}\n")
        
        
        node_weights_filename = os.path.join(output_dir, f"{node_weights_file_prefix}{file_num}{file_suffix}")
        
     #   with open(node_weights_filename, 'w') as node_weights_file:
      #      for node, data in G.nodes(data=True):
       #         node_weights_file.write(f"{node},{data['weight']}\n")
             
         # Save graph file info to CSV
        csv_filename = os.path.join(csv_dir, 'Beispiel_graph_info.csv')
        file_exists = os.path.isfile(csv_filename)
        with open(csv_filename, 'a', newline='') as csvfile:  # 'a' mode for appending
            writer = csv.writer(csvfile)
            if not file_exists:
                writer.writerow(['graph_file', 'node_weights_file', 'nodes', 'edges'])
            for i in range(3):
                graph_filename = os.path.join(output_dir, f"{file_prefix}{file_num}_{i}{file_suffix}")
                writer.writerow([graph_filename, n, m])
