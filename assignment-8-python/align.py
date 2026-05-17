#!/usr/bin/env python

import random # for seed, random
import sys    # for stdout



################################### TEST PART ##################################
################################################################################

# Tests align strands and scores
# Parameters types:
#    score          =  int   example: -6
#    plusScores     = string example: "  1   1  1"
#    minusScores    = string example: "22 111 11 "
#    strandAligned1 = string example: "  CAAGTCGC"
#    strandAligned2 = string example: "ATCCCATTAC"
#
#   Note: all strings must have same length
def test(score, plusScores, minusScores, strandAligned1, strandAligned2):
    print("\n>>>>>>START TEST<<<<<<")

    if testStrands(score, plusScores, minusScores, strandAligned1, strandAligned2):
        sys.stdout.write(">>>>>>>Test SUCCESS:")
        sys.stdout.write("\n\t\t" + "Score: "+str(score))
        sys.stdout.write("\n\t\t+ " + plusScores)
        sys.stdout.write("\n\t\t  " + strandAligned1)
        sys.stdout.write("\n\t\t  " + strandAligned2)
        sys.stdout.write("\n\t\t- " + minusScores)
        sys.stdout.write("\n\n")
    else:
        sys.stdout.write("\t>>>>!!!Test FAILED\n\n")


# converts character score to int
def testScoreToInt(score):
    if score == ' ':
        return 0
    return int(score)


# computes sum of scores
def testSumScore(scores):
    result = 0
    for ch in scores:
        result += testScoreToInt(ch)
    return result


# test each characters and scores
def testValidateEach(ch1, ch2, plusScore, minusScore):
    if ch1 == ' ' or ch2 == ' ':
        return plusScore == 0 and minusScore == 2
    if ch1 == ch2:
        return plusScore == 1 and minusScore == 0
    return plusScore == 0 and minusScore == 1


# test and validates strands
def testStrands(score, plusScores, minusScores, strandAligned1, strandAligned2):
    if len(plusScores) != len(minusScores) or len(minusScores) != len(strandAligned1) or len(strandAligned1) != len(
            strandAligned2):
        sys.stdout.write("Length mismatch! \n")
        return False

    if len(plusScores) == 0:
        sys.stdout.write("Length is Zero! \n")
        return False

    if testSumScore(plusScores) - testSumScore(minusScores) != score:
        sys.stdout.write("Score mismatch to score strings! TEST FAILED!\n")
        return False
    for i in range(len(plusScores)):
        if not testValidateEach(strandAligned1[i], strandAligned2[i], testScoreToInt(plusScores[i]),
                                testScoreToInt(minusScores[i])):
            sys.stdout.write("Invalid scores for position " + str(i) + ":\n")
            sys.stdout.write("\t char1: " + strandAligned1[i] + " char2: " +
                             strandAligned2[i] + " +" + str(testScoreToInt(plusScores[i])) + " -" +
                             str(testScoreToInt(minusScores[i])) + "\n")
            return False

    return True

######################## END OF TEST PART ######################################
################################################################################


# Computes the score of the optimal alignment of two DNA strands.
def findOptimalAlignment(strand1, strand2, memo):
    if(strand1,strand2) in memo:
        return memo[(strand1,strand2)]

    if len(strand1) == 0: 
        return {"score": len(strand2) * -2, "s1": " " * len(strand2), "s2": strand2}
    if len(strand2) == 0: 
        return {"score": len(strand1) * -2, "s1": strand1, "s2": " " * len(strand1)}

    bestWith = findOptimalAlignment(strand1[1:], strand2[1:],memo)
    
    if strand1[0] == strand2[0]:
        res = {"score": bestWith["score"] + 1, "s1": strand1[0] + bestWith["s1"], "s2": strand2[0] + bestWith["s2"]}
        memo[(strand1,strand2)] = res
        return res

    best = {
        "score" : bestWith["score"] - 1,
        "s1" : strand1[0] + bestWith["s1"],
        "s2" : strand2[0] + bestWith["s2"]
    }

    currRes1 = findOptimalAlignment(strand1, strand2[1:],memo)
    if currRes1["score"] - 2 > best["score"]:
        best = {
            "score": currRes1["score"] - 2,
            "s1": " " + currRes1["s1"],
            "s2": strand2[0] + currRes1["s2"]
        }

    currRes2 = findOptimalAlignment(strand1[1:], strand2,memo)
    if currRes2["score"] - 2 > best["score"]:
        best = {
            "score": currRes2["score"] - 2,
            "s1": strand1[0] + currRes2["s1"],
            "s2": " " + currRes2["s2"]
        }
    memo[(strand1,strand2)] = best
    return best
        
         
	

# Utility function that generates a random DNA string of
# a random length drawn from the range [minlength, maxlength]
def generateRandomDNAStrand(minlength, maxlength):
	assert minlength > 0, \
	       "Minimum length passed to generateRandomDNAStrand" \
	       "must be a positive number" # these \'s allow mult-line statements
	assert maxlength >= minlength, \
	       "Maximum length passed to generateRandomDNAStrand must be at " \
	       "as large as the specified minimum length"
	strand = ""
	length = random.choice(xrange(minlength, maxlength + 1))
	bases = ['A', 'T', 'G', 'C']
	for i in xrange(0, length):
		strand += random.choice(bases)
	return strand

# Method that just prints out the supplied alignment score.
# This is more of a placeholder for what will ultimately
# print out not only the score but the alignment as well.
def printAlignment(score, out = sys.stdout):
	out.write("Optimal alignment score is " + str(score) + "\n")

# Unit test main in place to do little more than
# exercise the above algorithm.  As written, it
# generates two fairly short DNA strands and
# determines the optimal alignment score.
#
# As you change the implementation of findOptimalAlignment
# to use memoization, you should change the 8s to 40s and
# the 10s to 60s and still see everything execute very
# quickly.
def main():
    test(-4, "  11 1 1 11 ", "12  2 2 1  2", "G ATCG GCAT ", "CAAT GTGAATC")

    while True:
        sys.stdout.write("Generate random DNA strands? ")
        user_input = sys.stdin.readline().strip().lower()
        if user_input == "no": 
            break
        
        s1 = generateRandomDNAStrand(150, 200)
        s2 = generateRandomDNAStrand(150, 200)

        memo = {} 
        
        result_map = findOptimalAlignment(s1, s2, memo)

        final_score = result_map["score"]
        aligned_s1 = result_map["s1"]
        aligned_s2 = result_map["s2"]

        plus_list = []
        minus_list = []
        
        for char1, char2 in zip(aligned_s1, aligned_s2):
            if char1 == ' ' or char2 == ' ':
                plus_list.append(" ")
                minus_list.append("2")
            elif char1 == char2:
                plus_list.append("1")
                minus_list.append(" ")
            else:
                plus_list.append(" ")
                minus_list.append("1")

        plus_final = "".join(plus_list)
        minus_final = "".join(minus_list)

        test(final_score, plus_final, minus_final, aligned_s1, aligned_s2)
if __name__ == "__main__":
   main()