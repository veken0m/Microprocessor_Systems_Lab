;moving_average.s
; By Michael Lagace (260372922) and Georges Krinker (260369844)
; ECSE 426
; Moving Average Filter v.1.1 OPTIMIZED
; Latest Recorded Execution time: 1.04�s

	AREA	FILTERCODE, CODE
	EXPORT moving_average_test
	EXPORT moving_average
	EXPORT moving_average_init
	
	; Below you will find a map of how we store our state. We use the stack, initially referenced by the stack pointer (SP).
	; We store a window of d size, always replacing the oldest data point with the newest
	
;	-------------------------------- 
;	|							   |
;	|			Index			   |    // The index is used to keep track of the oldest data point
;	|------------------------------|
;	|		'Old Moving Average'   |			 
;	|------------------------------|
;	|			data_d-1		   |
;	|------------------------------|							   
;	|			data_d-2		   |
;	|				.			   |
;	|				.			   |
;	|				.			   |
;	|------------------------------|
;	|			data_1			   |
;	|------------------------------|
;	|			data_0			   |
;	|------------------------------|
;					|
;					|
;					V 	STACK GROWS DOWNWARDS...
;
moving_average_test

	; allocate our state
    PUSH     {R5, R6, R12}          ; Preserve R5 and R6
	MOV      R0, #depth 			; Store depth in R0
    ADD		 R5, R0, #2 			; store d+2 in R5 which will be the space we'll allocate in our stack
	MOV		 R0, #4					; Now store 4 in R0
	MOV      R1, R13				; Move the SP onto R1.
	MLS      R5, R0, R5, R1			; Multiply and subtract, hence allocating our state
	MOV		 R0, R5					; put the top (geographical bottom) of the stack into R0 for parameter passing
	BL 	     moving_average_init	; go to the init program with the top of the stack in R0 for initialization 

inited	

	; Fetch new data point from test vector and check whether we're done...This check will be later removed...
	LDR		 R6, =test_vector 		; Fetch the new data point from the test vector
	ADD      R12, R6, #2048			; Get the last address of the test vector (will be removed in final version)

load_new

	; Load the new number, see if we're done (will be removed), 
	VLDR.F32 S1, [R6, #0]      
	BVS      load_new       		; if next value is bad(NaN, INF, OVF) skip it
    CMP      R6, R12				; else, check if we're done (remember, this won't be there in the final version!)
	BEQ      done					; if we've processed all of our values we're done...
	ADD 	 R6, R6 ,#4				; else point to the enxt data point in the vector
	MOV		 R0, R5					; safely store our stack pointer in R5 so we know it won't be fucked with
	BL 		 moving_average			; go calculate our new MA with that new data point
	
done

    POP      {R12, R6,R5}			; Revert changes
	BX R14							; return
	
moving_average_init

	; Set R2 as zero for storing and R3 a counter for looping
	MOV	 	 R3, #0
	MOV		 R2, #0
	
	
set_to_zero

	; initlialize all words in memory to 0
	STR 	 R2, [R0,#0]        	; Store 0 where R0 is pointing
	CMP		 R3, #depth				; See if the counter reached depth
	ADD  	 R3,R3,#1				; Add 1 to the counter
	ADD  	 R0,R0,#4				; move R0 to the next Word
	BLT      set_to_zero			; if we haven't filled depth words with zero, loop back
	STR 	 R2, [R0,#0]			; once we are done  store 0 in the position that will hold the moving average
	STR 	 R2, [R0,#4]			; also store 0 where we're storing an index
	MOV      R2, #depth				; set R2 to depth (for float conversion)
	VMOV	 S4, R2					; Put R2 in a float register
	VCVT.F32.U32 S4,S4				; Convert it into float. This will be used for division.
	
	; Store 1/d for performance gain in a arguement register
	VMOV.F32 S5, #1.0				; store 1 in float in S5
	VDIV.F32 S3, S5,S4				; store 1/d in S3
	BX		 R14     				; done with init, return

moving_average
	
	; Registers for Arithmetic such as depth and literal 4
	MOV	      R1, #4			; using R1 to store 4
	MOV 	  R2, #depth		; using R2 to store depth
	
	; Loading the index and pointing to the oldest data point stored in memory
	MLA       R2, R2, R1, R1	; R2 stores (d*4 + 4)
	ADD		  R2, R0, R2		; R0 + (d+1)*4 where R0 is the pointer to the top of the stack
	LDR       R3, [R2,#0]		; Load the data from R0 + (d+1)*4 (loading index)
	MLA       R1, R3, R1, R0 	; Make R1 point to stack pointer + index*4
	
	; Moving Average Algorithm
	VLDR.F32  S2, [R1,#0]		; Load the oldest data point onto S2
	VSUB.F32  S8,S1,S2			; Newest data point - oldest data point
	VMUL.F32  S8,S8,S3			; Divide the previous result by depth, storing it in S8
	VLDR.F32  S0,[R2,#-4]		; Load the 'old' moving average from memory
	VADD.F32  S0,S0,S8			; Add the 'old' moving average to S8; store it in S0 for return
	
	; Storing the new moving average and replacing the oldest data point with the newest
	VSTR.F32  S0,[R2,#-4]   	; Overwrite moving average with the new calculated moving average
	VSTR.F32  S1, [R1, #0]  	; Swap the oldest data point with the newest
	
	; incrementing the index and implementing the manual 'wrap around'
	ADD		  R3, R3, #1		; increment index by 1
	CMP		  R3, #depth		; if it's equal to the depth, it should have been 0
	MOVEQ	  R3, #0			; so yeah, move 0 if it was equal to d (reset the index)
	STR       R3, [R2,#0]		; store the incremented index which is stored in R2.
 	BX 	  	  R14 	    	    ; if there are still values to process, jump to shift loop
	

	LTORG
	
; Here are 512 samples of two periods of a sine wave with some noise
; added. Put this in your code somewhere.

	align 4
depth EQU 3

	align 4
test_vector
	DCFS 0.08592903,  0.02212529,  0.09950619, -0.0108313,  0.13294764
    DCFS 0.25890474,  0.25415575,  0.09538677,  0.12099221,  0.23192241	
	DCFS 0.18648198,  0.31892529,  0.47740761,  0.25099351,  0.36026869
	DCFS 0.26504675,  0.28061609,  0.31790071,  0.53650223,  0.42863947
	DCFS 0.43899463,  0.53329674,  0.49957499,  0.29960684,  0.62825964
	DCFS 0.47393765,  0.72407857,  0.68470216,  0.32272165,  0.47456111
	DCFS 0.53300641,  0.73290661,  0.61464151,  0.65159655,  0.73107554
	DCFS 0.62988432,  0.85276965,  0.93751523,  0.78004879,  0.98529152
	DCFS 0.97747597,  0.91960734,  0.8272955,  0.85072539,  0.79780207
	DCFS 0.88061353,  1.00514581,  0.75904255,  0.93672152,  0.8955957 
	DCFS 0.9475102,  0.92700772,  0.940373,  1.03338592,  1.0690618 
	DCFS 0.76693842,  0.99276902,  0.94397206,  0.70166733,  1.12673158
	DCFS 1.0766858,  1.11857439,  0.76839692,  1.10117313,  1.07000066
	DCFS 1.19059609,  1.05022401,  0.8625725,  1.05638471,  1.05092837
	DCFS 0.80232048,  0.89123441,  1.08446397,  0.96684372,  1.01070733
	DCFS 1.01457979,  0.86057199,  0.9260153,  0.89071756,  0.93559416
	DCFS 0.93848433,  0.96486639,  0.86626469,  0.89794781,  1.05894003
	DCFS 0.83777514,  0.799143,  0.92121354,  0.61458628,  0.78656967
	DCFS 0.79780562,  0.75807397,  0.75558723,  0.54779481,  0.6025397 
	DCFS 0.70598194,  0.70728358,  0.51311171,  0.65233976,  0.71348255
	DCFS 0.92832165,  0.6878424,  0.61659262,  0.6118942,  0.50752985
	DCFS 0.50992376,  0.55998735,  0.73937153,  0.43579655,  0.45980055
	DCFS 0.44345413,  0.41734511,  0.40194096,  0.2833624,  0.4063148 
	DCFS 0.30051264,  0.10508782,  0.22251119,  0.12953323,  0.21608694
	DCFS 0.17841934,  0.12676439,  0.06408548,  0.08878611,  0.24381251
	DCFS -0.0497391, -0.04089374,  0.00944747, -0.01055316,  0.12514643
	DCFS 0.03767973,  0.14122673, -0.13230165, -0.03657484, -0.09552386
	DCFS -0.04417419, -0.31061288, -0.23582823, -0.20517205, -0.20510399
	DCFS -0.2723238, -0.22813513, -0.27004971, -0.24541842, -0.32942958
	DCFS -0.47224395, -0.47772389, -0.32467069, -0.61136217, -0.47959858
	DCFS -0.29075186, -0.47296127, -0.64781434, -0.54334126, -0.48503317
	DCFS -0.47790007, -0.59413339, -0.63631242, -0.83330625, -0.75656974
	DCFS -0.81119314, -0.81037712, -0.74760882, -0.67454263, -0.67974345
	DCFS -0.63699064, -0.84512825, -0.81231346, -0.8258597, -0.91654472
	DCFS -1.05207554, -0.97038703, -0.82552881, -0.84944475, -0.9189383 
	DCFS -0.99649215, -0.97410076, -0.90254266, -0.81163975, -0.83105063
	DCFS -0.87264251, -1.00789201, -0.98038126, -0.91755727, -1.10933201
	DCFS -0.94113501, -1.12594104, -1.14647258, -0.83413254, -1.02645216
	DCFS -1.1823497, -1.11187273, -0.90603664, -0.90642182, -1.0727779 
	DCFS -0.89083546, -0.86417021, -0.8729863, -1.11047168, -1.00936907
	DCFS -0.92640683, -1.08891043, -1.08040699, -0.85724497, -1.20326837
	DCFS -0.88174752, -0.95384313, -0.78670442, -0.78492746, -0.72484669
	DCFS -0.8672389, -0.80170645, -0.78155496, -1.00169391, -0.92766591
	DCFS -0.84152899, -0.91335288, -0.89644104, -0.94539175, -0.71189777
	DCFS -0.53871044, -0.76377817, -0.66378316, -0.68702008, -0.74882739
	DCFS -0.86567027, -0.6271067, -0.63269012, -0.52034284, -0.59770931
	DCFS -0.65392025, -0.67862434, -0.53056648, -0.47938435, -0.32147218
	DCFS -0.6288469, -0.33758951, -0.36191514, -0.37327857, -0.25493203
	DCFS -0.47535105, -0.31534574, -0.1207113, -0.30879161, -0.28142109
	DCFS -0.27066238, -0.22684368, -0.18753942, -0.11172956, -0.15196723
	DCFS -0.16821618, -0.00846969, -0.44201692, -0.06498097, -0.11555364
	DCFS -0.03858682,  0.08736418, -0.01128483,  0.00758189, -0.10405749
	DCFS 0.03822799,  0.19903459,  0.34444131,  0.25288869,  0.27398969
	DCFS 0.29372364,  0.22501903,  0.23059065,  0.29302512,  0.37587254
	DCFS 0.26500339,  0.26894622,  0.46205866,  0.46706753,  0.33188312
	DCFS 0.60837998,  0.43678215,  0.4154284,  0.5370038,  0.59648446
	DCFS 0.41920932,  0.55004429,  0.61278746,  0.62783244,  0.39066801
	DCFS 0.55501883,  0.75778801,  0.83818988,  0.84526305,  0.57641503
	DCFS 0.68171609,  0.84064167,  0.66458682,  0.89657408,  0.770993  
	DCFS 0.83263188,  0.98264423,  0.79150081,  0.9759097,  1.16352611
	DCFS 1.00354853,  0.95257804,  0.77995684,  0.97230506,  1.02649338
	DCFS 0.98358269,  0.97950757,  0.97794278,  0.94069165,  0.95101166
	DCFS 1.02242817,  0.99605123,  0.85828576,  0.96052696,  0.89422924
	DCFS 0.90212676,  0.85674143,  0.95802827,  1.13379027,  1.07992794
	DCFS 1.09893546,  1.17522581,  1.00079189,  0.97527396,  0.89375415
	DCFS 0.94418654,  1.04458799,  1.01869718,  1.05499114,  1.02075999
	DCFS 1.00970608,  0.87849562,  0.81982791,  0.94148725,  0.95585303
	DCFS 0.96926905,  0.80600141,  0.81209789,  0.94832471,  0.87101787
	DCFS 0.87183794,  1.00122,  0.77596842,  0.9509005,  0.74512479
	DCFS 0.80912414,  0.81877988,  0.73121438,  0.79424976,  0.87783741
	DCFS 0.81177073,  0.70801304,  0.76066024,  0.57610546,  0.60002746
	DCFS 0.85308888,  0.48398456,  0.47924416,  0.57193751,  0.44821183
	DCFS 0.69390595,  0.31373217,  0.35765118,  0.46131132,  0.30140197
	DCFS 0.52122871,  0.39281622,  0.35277752,  0.3383714,  0.31485444
	DCFS 0.22489388,  0.28725148,  0.27683702,  0.2838226,  0.22633248
	DCFS 0.29411665,  0.19281683,  0.1577748,  0.06227887,  0.22218783
	DCFS -0.03993226,  0.0094547,  0.17242654, -0.0973318,  0.19217769
	DCFS -0.13320982, -0.20025609,  0.0303777, -0.08444563, -0.02330861
	DCFS -0.1848621, -0.12007541, -0.26632632, -0.09359267, -0.33058211
	DCFS -0.24861711, -0.29440927, -0.23252577, -0.34761653, -0.31441674
	DCFS -0.30781684, -0.34047493, -0.37270063, -0.5251593, -0.61800097
	DCFS -0.39295402, -0.4259651, -0.5243687, -0.64308784, -0.56339936
	DCFS -0.48761924, -0.53545063, -0.62692623, -0.63370058, -0.60900753
	DCFS -0.66284785, -0.66018932, -0.72102935, -0.75612769, -0.78605497
	DCFS -0.6406125, -0.99464595, -0.87116586, -0.70678121, -0.7740983 
	DCFS -0.76852141, -0.90531186, -0.86803616, -0.98733561, -1.0663905 
	DCFS -1.00376915, -0.96314222, -0.87193907, -1.02049914, -1.02746856
	DCFS -0.8897155, -1.06050522, -1.07184021, -0.93628965, -0.99381072
	DCFS -0.99570915, -1.08668644, -0.92676673, -1.24130337, -1.10419271
	DCFS -0.91456225, -1.02134733, -0.97785416, -0.82882931, -0.99054686
	DCFS -0.98060715, -1.05969037, -1.10755376, -0.92834049, -1.2763575 
	DCFS -0.85814646, -1.14290736, -0.851239, -1.12969736, -0.93161632
	DCFS -0.84648253, -0.92796699, -0.88845193, -1.02907291, -0.95206476
	DCFS -0.94118686, -0.85826659, -0.84965335, -1.03962173, -0.81750386
	DCFS -0.89186038, -0.66356923, -0.60450947, -0.86940991, -0.8843178 
	DCFS -0.76959244, -0.76949202, -0.61555289, -0.81288649, -0.84365818
	DCFS -0.62391178, -0.71125092, -0.67673037, -0.84159866, -0.57795855
	DCFS -0.79537762, -0.79692759, -0.69233167, -0.6091777, -0.60904592
	DCFS -0.43062694, -0.46961604, -0.51369777, -0.49458963, -0.39318427
	DCFS -0.45222555, -0.26583435, -0.53668158, -0.42329538, -0.35615139
	DCFS -0.44690349, -0.19705927, -0.33855893, -0.36074107, -0.16343595
	DCFS -0.17484013, -0.1421301, -0.13777421, -0.09818376, -0.10141892
	DCFS -0.07593607,  0.11331186

	
	END
	
	