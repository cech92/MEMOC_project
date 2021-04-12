import sys
import os
import random

def main(argv):
    print(argv)
    N = int(argv[1])

    print(N)

    test_n = 1
    path = "inputs/random_size_" + str(N) + "_coordinates_n_" + str(test_n) + ".txt"
    while os.path.exists(path):
        test_n += 1
        path = "inputs/random_size_" + str(N) + "_coordinates_n_" + str(test_n) + ".txt"
    outputFile = open(path, "w")
    i = 0
    coords = []
    while i < N:
        coord = (random.randint(0, N), random.randint(0, N))
        if coord not in coords:
            coords.append(coord)
            outputFile.write(str(coord[0]) + " " + str(coord[1]) + "\n")
            i += 1
    outputFile.close()


if __name__ == '__main__':
    main(sys.argv)
