# This is a program to create DIMACS SAT-instances file for the proper colouring problem

import numpy as np
import itertools as it

def inputFile(filePath):
    vertices = [[] for i in range(2)]
    with open(filePath) as f:
        for line in f.readlines():
            s = line.split(' ')
            vertices[0].append(int(s[0]))
            vertices[1].append(int(s[1]))
    vertices = np.array(vertices)
    return (vertices)

def chromaticNumSAT(chromaticNum, vertices):
    clauses = []
    verticeNum = np.max(vertices) + 1
    # Rule 1
    # Each vertex must be coloured
    for i in range(verticeNum):
        tmpClause = ''
        for j in range(chromaticNum):
            tmpClause += str(i + j * verticeNum + 1) + ' '
        tmpClause += '0'
        clauses.append(tmpClause)
    # Rule 2
    # Each vertex only be coloured by a single colour
    for i in range(verticeNum):
        for j in range(chromaticNum):
            for k in range(j + 1, chromaticNum):
                tmpClause = ''
                tmpClause += str(-(i + j * verticeNum + 1)) + ' '
                tmpClause += str(-(i + k * verticeNum + 1)) + ' '
                tmpClause += '0'
                clauses.append(tmpClause)
    # Rule 3
    # Each edge cannot be coloured with the same colour
    for i in range(np.shape(vertices)[1]):
        for j in range(chromaticNum):
            tmpClause = ''
            tmpClause += str(-(vertices[0, i] + j * verticeNum + 1)) + ' '
            tmpClause += str(-(vertices[1, i] + j * verticeNum + 1)) + ' '
            tmpClause += '0'
            clauses.append(tmpClause)
    return(clauses)

def outputChromaticNumFile(fileNum, chromaticNum, vertices, clauses):
    verticeNum = np.max(vertices) + 1
    clausesLen = len(clauses)
    with open('GraphR%s_ChromaticNum%s.cnf'%(fileNum, chromaticNum), mode = 'w', encoding = 'utf-8') as f:
        f.write('c This is a DIMACS SAT-instances file to check the chromatic number\n')
        f.write('c \n')
        f.write('c The file Name               : GraphR%s.txt\n'%(fileNum))
        f.write('c The number of vertices      : %s\n'%(verticeNum))
        f.write('c \n')
        f.write('c The chosen number of colour : %s\n' % (chromaticNum))
        f.write('c \n')
        f.write('p cnf %s %s\n'%(verticeNum * chromaticNum, clausesLen))
        for i in range(clausesLen):
            f.write(clauses[i] + '\n')
    return()

def adjacencyMatrix(vertices):
    verticeNum = np.max(vertices) + 1
    adjacencyMat = np.zeros([verticeNum, verticeNum]).astype(int)
    for i in range(np.shape(vertices)[1]):
        adjacencyMat[vertices[0, i], vertices[1, i]] = 1
        adjacencyMat[vertices[1, i], vertices[0, i]] = 1
    return(adjacencyMat)

def cliqueNumSAT(cliqueNum, vertices):
    clauses = []
    verticeNum = np.max(vertices) + 1
    adjacencyMat = adjacencyMatrix(vertices)
    # Rule 1
    # Two distinct vertices that are member of a clique must be connected by an edge
    for i in range(verticeNum):
        for j in range(i + 1, verticeNum):
            edgeNum = 0
            if adjacencyMat[i, j] == 0:
                tmpClause = str(-(i + 1)) + ' ' + str(-(j + 1)) + ' 0'
                clauses.append(tmpClause)
                edgeNum += 1
    # Rule 2
    # Each vertex of the graph that is connected to all the vertices of a maximal clique must be also a member of the clique
    for i in range(verticeNum):
        tmpClause = str(i + 1) + ' '
        for j in range(verticeNum):
            if adjacencyMat[i, j] == 0 and i != j:
                tmpClause +=  str(j + 1) + ' '
        tmpClause += '0'
        clauses.append(tmpClause)
    # Rule 3
    # Constraint the size of a maximal clique
    groups = list(it.combinations(np.arange(verticeNum) + 1, verticeNum - cliqueNum + 1))
    for i in range(np.shape(groups)[0]):
        tmpClause = ''
        for j in range(len(groups[i])):
            tmpClause += str(groups[i][j]) + ' '
        tmpClause += '0'
        clauses.append(tmpClause)
    return(clauses)

def outputCliqueNumFile(fileNum, cliqueNum, vertices, clauses):
    verticeNum = np.max(vertices) + 1
    clausesLen = len(clauses)
    with open('GraphR%s_CliqueNum%s.cnf'%(fileNum, cliqueNum), mode = 'w', encoding = 'utf-8') as f:
        f.write('c This is a DIMACS SAT-instances file to check the clique number\n')
        f.write('c \n')
        f.write('c The file Name                    : GraphR%s.txt\n'%(fileNum))
        f.write('c The number of vertices           : %s\n'%(verticeNum))
        f.write('c \n')
        f.write('c The chosen number of clique size : %s\n' % (cliqueNum))
        f.write('c \n')
        f.write('p cnf %s %s\n'%(int(verticeNum), clausesLen))
        for i in range(clausesLen):
            f.write(clauses[i] + '\n')
    return()

def main():
    chromaticNum = [4, 4, 3, 3, 3]
    cliqueNum = [3, 3, 3, 3, 3]
    vertices = {}
    chromaticNumClauses = {}
    cliqueNumClauses = {}
    for i in range(5):
        vertices[i] = inputFile('GraphR%s.txt'%(i + 1))
        chromaticNumClauses[i] = chromaticNumSAT(chromaticNum[i], vertices[i])
        outputChromaticNumFile(i + 1, chromaticNum[i], vertices[i], chromaticNumClauses[i])
        cliqueNumClauses[i] = cliqueNumSAT(cliqueNum[i], vertices[i])
        outputCliqueNumFile(i + 1, cliqueNum[i], vertices[i], cliqueNumClauses[i])
    return()

main()