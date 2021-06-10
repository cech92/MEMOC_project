import math
import sys
import os
import random

def main(argv):
    N = int(argv[1])

    test_n = 1
    path = "inputs/input_size_" + str(N) + "_n_" + str(test_n) + ".txt"
    while os.path.exists(path):
        test_n += 1
        path = "inputs/input_size_" + str(N) + "_n_" + str(test_n) + ".txt"
    outputFile = open(path, "w")
    outputFile.write(str(N) + "\n")
    i = 0
    coords = []
    while i < N:
        coord = (random.randint(0, N), random.randint(0, N))
        if coord not in coords:
            coords.append(coord)
            # outputFile.write(str(coord[0]) + " " + str(coord[1]) + "\n")
            i += 1

    for coord1 in coords:
        for coord2 in coords:
            if coord1 == coord2:
                outputFile.write(str(0.0) + " ")
            else:
                outputFile.write(str(round(math.sqrt((coord1[0] - coord2[0]) ** 2 + (coord1[1] - coord2[1]) ** 2), 4)) + " ")
        outputFile.write("\n")
    outputFile.close()


if __name__ == '__main__':
    main(sys.argv)
