#include "HybridSelfIndex.h"

bool TRACE_L = false;		// true: print all details for console
bool TEST_IND = false;		// true: apply exhaustive test
bool RUN_EXP = 1;		// true: run the experiments for locate
uint REPET = 100;
uint MAX_M = 160;

int mode, mode2;
char text[100];

// Structure with all globals parameters program
typedef struct {
	uchar *seq;				// original sequence (1 byte for symbol)
	ulong n;				// Length of original Text
	uint M;					// maximum pattern length
	char parserFile[400];	// file with the parser
	char prefixStore[300];	// directory to save/load the data structure (files *.dlhi)
	char dirPatt[300];		// directory to read the patterns
	char prefixResult[300];	// Prefix name of results files in the experiments (folder/prefix)

	HybridSelfIndex *index;

	uchar** patt;			// data structure are for test and experiments
} ParProgL;

void runExperiments(ParProgL *par);
void testExtractLoad(ParProgL *par);
void testLocateLoad(ParProgL *par);

// /home/ferrada/data/documents/alabarda/hi_simple/alabardaXL/ /home/ferrada/data/documents/alabarda/FMI_test/alabardaXL/ /home/ferrada/data/documents/alabarda/hi_simple/alabardaXL/res/ 20 100
// /home/ferrada/data/repetitive/cere/hi_simple/461MiB_M40_SA32/ /home/ferrada/data/repetitive/cere/461MiB_M80_SA32_ISA128/ /home/ferrada/data/repetitive/cere/hi_simple/461MiB_M40_SA32/res/res 160 3000
// /home/ferrada/data/repetitive/influ/hsi_simple/M40_SA32/ /home/ferrada/data/repetitive/influ/patt/ /home/ferrada/data/repetitive/cere/hi_simple/461MiB_M40_SA32/res/ 160 3000

int main(int argc, char *argv[]) {
	ParProgL *par = new ParProgL();
	char fileName[400];

	//cout << "Running load_hsi_ms.cpp ..." << endl;


	if(argc != 9){
		cout << "ERRORR !! " << endl;
		cout << "load_hsi_ms's usage requires 2 parameters:" << endl;
		cout << "<dirStore> the directory where to read the structures" << endl;
		cout << "<diPatterns> the directory where to read the file of patterns" << endl;
		cout << "<prefixResult> Prefix name of results files in the experiments (folder/prefix)" << endl;
		cout << "<MAX_M> Maximum pattern length for locate experiment" << endl;
		cout << "<REPET> Number of patterns for locate experiment" << endl;
		exit(1);
	}

	HybridSelfIndex::TRACE = false;
	HybridSelfIndex::SHOW_SIZE = false;

	strcpy(par->prefixStore, "");
	strcpy(par->prefixResult, "");

	strcpy(par->prefixStore, argv[1]);
	strcpy(par->dirPatt, argv[2]);
	strcpy(par->prefixResult, argv[3]);
	MAX_M = atoi(argv[4]);
	REPET = atoi(argv[5]);
	mode = atoi(argv[6]);
	mode2 = atoi(argv[7]);
	strcpy(text,argv[8]);

	/*cout << "load_hsi_ms parameters..." << endl;
	cout << "Base Name path: " << par->prefixStore << endl;
	cout << "dirPatt: " << par->dirPatt << endl;
	cout << "prefixResult: " << par->prefixResult << endl;
	cout << "MAX_M: " << MAX				cout << *nOcc-nn << endl;
_M << endl;
	cout << "REPET: " << REPET << endl;*/

	par->index = new HybridSelfIndex(par->prefixStore);

	par->index->setNumThreads(mode);

	par->n = par->index->n;
	par->M = par->index->M;
	//cout << "n: " << par->n << endl;
	//cout << "M: " << par->M << endl;

	

	if (TEST_IND){
		// Loading FMI_TEST to create random patterns
		strcpy(fileName, "");
		strcpy(fileName, par->prefixStore);
		strcat(fileName, "fmi_test.test");
		cout << "loading the FMI_TEST in " << fileName << " ..." << endl;

		if (load_from_file(par->index->FMI_TEST, fileName)){
			//cout << " **  FMI length " << par->index->FMI_TEST.size() << endl;
			if (par->index->FMI_TEST.size() != par->n+1){
				cout << "ERROR. FMI length != n+1 = " << par->n+1 << endl;
				exit(1);
			}
			//cout << "====================================================" << endl;
			ulong sizeFMI = size_in_bytes(par->index->FMI_TEST);
			/*cout << " ### FMI_TEST size " << sizeFMI << " bytes = " << sizeFMI/(1024.0*1024.0) << " MiB = " << (float)sizeFMI*8.0/(float)par->n << " bpc" << endl;
			cout << "====================================================" << endl << endl;

			cout << "Test Locate()..." << endl;*/
			testLocateLoad(par);
			//cout << "Test Locate() OK !!" << endl;

			if (S_ISA <= 1024){
				cout << "Running test extract().." << endl;
				testExtractLoad(par);
				cout << "Test extract() OK !!" << endl;
			}else
				cout << " Take into account that Testing extract() will take a lot of time given the sapling-size of the inverse SA!" << endl;
		}else{
			cout << "There is not FMI_TEST created by the Hybrid Index in: " << fileName << endl;
			cout << "... you need to set HybridSelfIndex::CREATE_FMI_TEST = true; before to call the constructor, but build the FMI for the hole file takes considerable time!!" <<  endl;
		}
	}

	if (RUN_EXP){
		cout << "Experiments..." << endl;
		runExperiments(par);
		cout << "Experiments OK !!" << endl;
	}

	cout << "$$$$$$$$$$$$$$$$$$$$$" << endl;
	return 0;
}

void loadPatterns(ParProgL *par, uint m){
	char filePatt[400];
	char str[100];

	strcpy(filePatt, "");
	strcpy(filePatt, par->dirPatt);
	//cout << "Reading patterns from "<< filePatt << std::endl;

	par->patt = new uchar*[REPET];

	uint i, j;
	string line;
	ifstream input (filePatt);
	assert(input.good()); 				// check open
	if (input.is_open()){
		i = 0;
		while (input.good() && i < REPET){
			getline(input,line);
			par->patt[i] = new uchar[m+1];
			for(j=0; j<m; j++)
				par->patt[i][j] = (uchar)(line.at(j));
			//cout << i << " " << par->patt[i] << endl;
			par->patt[i][m] = '\0';
			i++;
		}
		input.close();
	} else{
		cout << "Unable to open file " << filePatt << endl;
		exit(-1);
	}

	/*cout << i << " patterns read !" << endl;
	if (i<REPET){
		REPET = i;
		cout << "The file contains only " << i << " patterns !" << endl;
	}*/
}

void runExperimentLocate(ParProgL *par, uint m){
	double t, avgTime;
	double avgnOcc;
	char aFile[400];
	char str[100];
	
	
	

	/*cout << "____________________________________________________" << endl;
	cout << "  Locate " << REPET << " patterns of length m = " << m << endl;*/
	avgTime = 0.0;
	avgnOcc = 0.0;
	/*cout << "algorithm " << "text " << "m " << "patt_id " <<"nOcc " << "tseq " << "tpo "<< "tso " << "tps ";
	for (int i = 0; i < 32; ++i){
		cout << "fo_" << i <<" ";
	}
	for (int i = 0; i < 32; ++i){
		cout << "so_" << i <<" ";
	}
	cout << endl;*/
	double tpss = 0, tseqs = 0;
	for (ulong k=0; k<REPET; k++){
		double ttt=0.0, tseq= 0.0, tpo = 0.0, tso = 0.0, tps = 0.0;
		ulong nOcc, *occ;
		vector<ulong> fo(32, 0);
		vector<ulong> so(32, 0);
		//cout << "k=" << k << endl;
		
		par->index->locate(par->patt[k], m, &nOcc, &occ, mode, mode2, tseq, tpo, tso, tps, fo, so);

		cout << tseq << " " << tpo << " " << tso << " " << tps << "\n";

		FILE *fp = fopen("res/res2.csv", "a+" );
		fprintf(fp, "locate_%d_%d %s %d %lu %lu %f %f %f %f ",mode,mode2, text, m, k, nOcc, tseq, tpo, tso, tps);
		for (int i = 0; i < 32; ++i){
			fprintf(fp, "%lu ", fo[i]);
		}
		for (int i = 0; i < 31; ++i){
			fprintf(fp, "%lu ", so[i]);
		}
		fprintf(fp, "%lu\n", so[31]);
		fclose(fp);
		tpss += tpo + tso + tps;
		tseqs += tseq;
		avgTime += tseq + tpo + tso + tps ;
		avgnOcc += nOcc;
		/*if (nOcc)
			delete [] occ;*/
	}
	cout << avgTime << " " << avgnOcc << endl;
	cout << tseqs << " " << tpss << endl;
	//ttt+=tloc+tfs;
	/*cout << "FINAL TOTAL TIME = " << (float)ttt/nREP << "s.\n";
	cout << "TOTAL TIME LOCATE = " << tloc/nREP << '\n';
	cout << "TOTAL TIME first-second occ = " << tfs/nREP << '\n';
	cout << "nOcc found : " << int(avgnOcc)/nREP << endl;
	avgnOcc /= (float)REPET;
	cout << "Average CPU time per execution: " << (float)(ttt/(nREP*REPET)) << " s" << endl;
	cout << "Average nOcc found : " << (float)avgnOcc/nREP << endl;
	cout << "Average CPU time per occurrence: " << (float)((ttt/(nREP*REPET))/(avgnOcc/nREP))*1000000.0 << " microS" << endl;
	cout << "Size : " << par->index->sizeDS*8.0/(float)par->n << endl;
	cout << "____________________________________________________" << endl;*/

	/*strcpy(aFile, par->prefixResult);
	strcpy(str, "");
	sprintf(str, "locate_%s_%d", text, m);
	strcat(aFile, str);
	cout << "Resume File: " << aFile << endl;*/

	/*FILE *fp = fopen(aFile, "a+" );
	// [text] [m] [mode=nThreads] [FMI_SA] [size bpc] [avgnOcc] [avg locate-time (mu)]
	fprintf
	fprintf(fp, "%s_sa%d_m%d_t%d %f %f %f %G %f %f\n",text, S_SA, m, mode, par->index->sizeDS/(1024.0*1024.0), par->index->sizeDS*8.0/(float)par->n, avgnOcc, (float)((ttt/(nREP*REPET))/(avgnOcc/nREP))*1000000.0, (float)((tloc/(nREP*REPET))/(avgnOcc/nREP))*1000000.0,(float)((tfs/(nREP*REPET))/(avgnOcc/nREP))*1000000.0);
	fclose(fp);*/
}

void runExperimentExtract(ParProgL *par, uint m){
	uchar *A;
	ulong k, randPos;
	double t, avgTime;
	char aFile[400];
	char str[100];

	cout << "____________________________________________________" << endl;
	cout << "  Extract " << REPET << " random strings of length m = " << m << endl;
	avgTime = 0.0;

	for (k=0; k<REPET; k++){
		randPos = rand() % (par->n-m-2);
		t = getTime_ms();
		par->index->extract(randPos, m, &A);
		t = getTime_ms() - t;
		avgTime += t/(double)REPET;
	}
	cout << "Average CPU time for extract() : " << avgTime*1000.0 << " Microseconds" << endl;
	cout << "Size : " << par->index->sizeDS*8.0/(float)par->n << endl;
	cout << "____________________________________________________" << endl;

	strcpy(aFile, par->prefixResult);
	strcpy(str, "");
	sprintf(str, "_hsi_extract_M%d_SA_%d_ISA_%d", par->index->M, S_SA, S_ISA);
	strcat(aFile, str);

	FILE *fp = fopen(aFile, "a+" );
	// [m] [M] [FMI_SA] [FMI_I_SA] [size (MiB)] [size (bpc)] [avg extract_time (us)]
	fprintf(fp, "%d %d %d %d %f %f %G\n", m, par->index->M, S_SA, S_ISA, par->index->sizeDS/(1024.0*1024.0), par->index->sizeDS*8.0/(float)par->n, avgTime*1000.0);
	fclose(fp);

	delete []A;
}

void runExperiments(ParProgL *par){
	//cout << "====================================================" << endl;

	uint TRUE_REP = REPET;
	loadPatterns(par, MAX_M);
	runExperimentLocate(par, MAX_M);
	/*REPET = TRUE_REP;

	for (uint m=20; m<=MAX_M; m*=2){
		loadPatterns(par, m);
		runExperimentLocate(par, m);
	}*/

	delete [] (par->patt);
}

void testExtractLoad(ParProgL *par){
	uchar *A;
	ulong i, t, randPos;
	uint m, Max = 100;
	string substr;

	for (m=5; m<Max; m+=5){
		cout << " ...testing patterns for m = " << m << endl;
		for (t=0; t<REPET; t++){
			randPos = rand() % (par->n-m-2);
			substr = sdsl::extract(par->index->FMI_TEST, randPos, randPos+m-1);
			//cout << t << ", substr = [" << substr << "]" << endl;
			par->index->extract(randPos, m, &A);

			for(i=0; i<m; i++){
				if ((int)(substr[i]) != (int)(A[i]) && (256+(int)(substr[i]) != (int)(A[i]))){
					cout << "ERROR: m=" << m << ", test=" << t << ", sp = " << par->patt[t] << endl;
					cout << "substr = [" << substr << "]" << endl;
					cout << "Different symbol in position " << i << ", substr[i] = [" << substr[i] << "] != A[i] = [" << A[i] << "]" << endl;
					cout << "(int)(substr[i]) = " << (int)(substr[i]) << ", (int)(A[i]) = " << (int)(A[i]) << endl;
					exit(1);
				}
			}
			delete [] A;
		}
	}
}

void testLocateLoad(ParProgL *par){
	ulong *A = nullptr;
	ulong i, t, nOcc, *occ, nPry, randPos;
	uint m;
	bit_vector B_TEST(par->n+1, 0);

	//for (m=par->index->M/3; m<=4*par->M; m*=par->index->M/2){
	for (m=par->index->M/3; m<=3*par->M; m+=par->index->M/3){
		uchar* pat = new uchar[m+1];
		cout << " ... testing patterns for m = " << m << endl;
		for (t=0; t<REPET; t++){
			//cout << "t =" << t << endl;

			randPos = rand() % (par->n-m-2);
			string query = sdsl::extract(par->index->FMI_TEST, randPos, randPos+m-1);
			size_t occs = sdsl::count(par->index->FMI_TEST, query.begin(), query.begin()+m);
			/*if (occs){
				A = new ulong[occs];dFMI = " << occs << endl;
				//cout << "Primary locations..." << endl;
				for(i=nPry=0; i<occs; i++){
					//if (par->index->isPrimaryT(locations[i], m)){
						B_TEST[locations[i]] = 1;
						A[nPry] = locations[i];
						nPry++;
						//cout << i << ", " << locations[i] << endl;
					//}//else
						//cout << "S " << i << ", " << locations[i] << endl;
				}
				//cout << endl;
			}*/

			strncpy((char*) pat, query.c_str(), m);
			//cout << "Pattern = [" << pat << "]" << endl;
			double ttt=0.0, tseq= 0.0, tpo = 0.0, tso = 0.0, tps = 0.0;
			//par->index->locate(pat, m, &nOcc, &occ,mode,mode2,tseq,tpo,tso,tps);

			if(nPry != nOcc){
				if (m>1){
					cout << "ERROR: m=" << m << ", test=" << t << ", occs with FMI_TEST = " << nPry << " != nOcc = " << nOcc << endl;
					cout << "patt = [";
					for(uint j=0; j<m; j++)
						cout << pat[j];
					cout << "]" << endl;
					for(i=0; i<nOcc; i++){
						if(B_TEST[occ[i]] == 0)
							cout << occ[i] << " not found with the FMI_TEST !" << endl;
						else
							B_TEST[occ[i]] = 0;
					}

					for(i=0; i<nPry; i++){
						if(B_TEST[A[i]] == 1)
							cout << A[i] << " not found with the Hybrid Index !" << endl;
					}
					exit(1);
				}else{
					cout << "WARNING: m=1, test=" << t << ", occs with FMI_TEST = " << nPry << " != nOcc = " << nOcc << endl;
					cout << "patt = [" << pat[0] << "], CODE = " << (int) (pat[0]) << endl;
				}
			}

			for(i=0; i<nOcc; i++){
				if(B_TEST[occ[i]] == 0){
					cout << "ERROR: m=" << m << ", test=" << t << endl;
					cout << "patt = [";
					for(uint j=0; j<m; j++)
						cout << pat[j];
					cout << "]" << endl;
					cout << occ[i] << " not found with the FMI_TEST !" << endl;
					exit(1);
				}
			}

			for(i=0; i<nOcc; i++)
				B_TEST[occ[i]] = 0;

			if (occs){
				delete [] occ;
				delete [] A;
			}

		}
	}
	delete [] (par->patt);
}
