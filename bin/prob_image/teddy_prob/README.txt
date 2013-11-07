//the layer containing all points without using box filter is denoted a layer 0, and its disparity as d0 in correspondence.
gnd_intersection, MST_intersection:
	count the number of instances of <d1, d0> in ground truth and the MST built in layer 0 as opposed to MST in layer 1 respectively.
largeGivenSmall_20:
	show the calculated Pr[d0|d2] in "20" and the true Pr[d0|d2] in "true20".
	I use gnd in layer 0 and MST in layer 2 to get true Pr[d0|d2].
	I use gnd in layer 0 and MST in layer 1,2 to get Pr[d1|d0], Pr[d2|d1] and to calculate the proposed Pr[d0|d2]
	I didn't use any good function to model the probability distribution. Only statistical facts are used in calculating Pr[d1|d0], Pr[d2|d1].

largeGivenSmall_20_Err:
	the error of  largeGivenSmall_20.

prob_gnd_1, prob_MST_1:
	the prob between layer 1 and layer0 using gnd and MST respectively.

smallGivenLarge_20_21_10...:
	Pr[d0|d2], Pr[d1|d2], Pr[d0|d1]

smallGivenLargeGND_10...:
	Pr[d0|d1] viewed from different perspective.