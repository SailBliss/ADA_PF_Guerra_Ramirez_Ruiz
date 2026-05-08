# ADA Practica Final Integradora

## Integrantes

- David Guerra
- Samuel Ramirez
- Isabela Ruiz

## Requisitos

- Compilador C++ con soporte para C++17.
- Dataset incluido en `data/WA_Fn-UseC_-Telco-Customer-Churn.csv`.

## Compilacion recomendada

Comando limpio para compilar todos los archivos fuente una sola vez:

```bash
g++ -std=c++17 -O2 -o ada_pf src/*.cpp
```

Nota sobre el enunciado: el comando escrito

```bash
g++ -std=c++17 -O2 -o ada_pf src/main.cpp src/*.cpp
```

incluye `src/main.cpp` dos veces, porque el patron `src/*.cpp` tambien lo expande. Usarlo literalmente puede causar doble definicion de `main()`. Por eso se recomienda `src/*.cpp`, que compila exactamente los archivos del proyecto una sola vez.

## Ejecucion

En Windows PowerShell:

```powershell
.\ada_pf.exe data\WA_Fn-UseC_-Telco-Customer-Churn.csv
```

En Linux/macOS:

```bash
./ada_pf data/WA_Fn-UseC_-Telco-Customer-Churn.csv
```

## Archivos generados

La ejecucion completa genera:

- `results/solicitudes_ordenadas.csv`: solicitudes ordenadas por `tenure` descendente.
- `results/busquedas_A.txt`: resultados de las cinco busquedas exactas por `tenure`.
- `results/mst_red.txt`: grafo, MST por Kruskal, verificacion del subgrafo de 5 nodos y justificacion codiciosa.
- `results/asignacion_bw.txt`: solucion de Mochila 0-1, backtracking, contraejemplo codicioso y analisis de complejidad.
