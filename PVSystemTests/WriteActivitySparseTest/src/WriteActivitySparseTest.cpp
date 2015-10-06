/*
 * main .cpp file for WriteActivitySparseTest
 *
 */


#include <columns/buildandrun.hpp>
#include <layers/Movie.hpp>
#include <io/ParamGroupHandler.hpp>
#include "TestNotAlwaysAllZerosProbe.hpp"
#include "TestAllZerosProbe.hpp"

class CustomGroupHandler : public ParamGroupHandler {
public:
   CustomGroupHandler() {}
   virtual ~CustomGroupHandler() {}
   virtual ParamGroupType getGroupType(char const * keyword) {
      ParamGroupType result = UnrecognizedGroupType;
      if (keyword==NULL) { result = UnrecognizedGroupType; }
      else if (!strcmp(keyword, "TestAllZerosProbe")) { result = ProbeGroupType; }
      else if (!strcmp(keyword, "TestNotAlwaysAllZerosProbe")) { result = ProbeGroupType; }
      else { result = UnrecognizedGroupType; }
      return result;
   }
   virtual BaseProbe * createProbe(char const * keyword, char const * name, HyPerCol * hc) {
      BaseProbe * addedProbe = NULL;
      bool matched = false;
      if (keyword==NULL) { addedProbe = NULL; }
      else if (!strcmp(keyword, "TestAllZerosProbe")) {
         matched = true;
         addedProbe = new TestAllZerosProbe(name, hc);
      }
      else if (!strcmp(keyword, "TestNotAlwaysAllZerosProbe")) {
         matched = true;
         addedProbe = new TestNotAlwaysAllZerosProbe(name, hc);
      }
      else { addedProbe = NULL; }
      if (matched && !addedProbe) {
         fprintf(stderr, "Rank %d process unable to create %s \"%s\".\n", hc->columnId(), keyword, name);
         exit(EXIT_FAILURE);
      }
      return addedProbe;
   }
}; // class CustomGroupHandler

int checkProbesOnExit(HyPerCol * hc, int argc, char * argv[]);

int main(int argc, char * argv[]) {
   int rank = 0;
   PV_Init* initObj = new PV_Init(&argc, &argv, false/*allowUnrecognizedArguments*/);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   char const * paramFile1 = "input/GenerateOutput.params";
   char const * paramFile2 = "input/TestOutput.params";
   char const * outputDir1 = "outputGenerate";
   char const * outputDir2 = "outputTest";
   int status = PV_SUCCESS;
   PV_Arguments * arguments = initObj->getArguments();
   if (arguments->getParamsFile()!=NULL) {
      if (rank==0) {
         fprintf(stderr, "%s should be run without the params file argument.\n", arguments->getProgramName());
         fprintf(stderr, "This test uses two hard-coded params files, %s and %s. The first generates an output pvp file, and the second checks whether the output is consistent with the input.\n",
               paramFile1, paramFile2);
      }
      MPI_Barrier(MPI_COMM_WORLD);
      exit(EXIT_FAILURE);
   }

   if (rank==0) {
      char const * rmcommand = "rm -rf outputGenerate outputTest";
      status = system(rmcommand);
      if (status != 0) {
         fprintf(stderr, "deleting old output directories failed: \"%s\" returned %d\n", rmcommand, status);
         exit(EXIT_FAILURE);
      }
   }

   ParamGroupHandler * customGroupHandler = new CustomGroupHandler;

   arguments->setParamsFile(paramFile1);
   status = rebuildandrun(initObj, NULL, NULL, &customGroupHandler, 1);
   if( status != PV_SUCCESS ) {
      fprintf(stderr, "%s: rank %d running with params file %s returned error %d.\n", arguments->getProgramName(), rank, paramFile1, status);
      exit(status);
   }

   arguments->setParamsFile(paramFile2);

   status = rebuildandrun(initObj, NULL, &checkProbesOnExit, &customGroupHandler, 1);
   if( status != PV_SUCCESS ) {
      fprintf(stderr, "%s: rank %d running with params file %s returned error %d.\n", arguments->getProgramName(), rank, paramFile2, status);
   }

   delete customGroupHandler;

   delete initObj;
   return status==PV_SUCCESS ? EXIT_SUCCESS : EXIT_FAILURE;
}

// check that the input layer has become nonzero at some point
// If the comparison layer ever has a nonzero value, TestAllZerosProbe
// should catch it and exit with an error.
// However, there are some bugs that could cause Movie to
// have all zeros in the activity.  In that case, OriginalMovie and
// GeneratedMovie would always be zero, so the comparison would always
// be zero, so the test would pass even though there's a bug.
//
// The problem of what to do if comparison reports zero when given
// nonzero input is best left for a different test.
int checkProbesOnExit(HyPerCol * hc, int argc, char * argv[]) {
   BaseLayer * layer = hc->getLayerFromName("OriginalMovie");
   assert(layer);
   HyPerLayer * originalMovieLayer = dynamic_cast<HyPerLayer *>(layer);
   assert(originalMovieLayer);
   int numProbes = originalMovieLayer->getNumProbes();
   assert(numProbes==1);
   LayerProbe * originalMovieProbe = originalMovieLayer->getProbe(0);
   assert(originalMovieProbe);
   TestNotAlwaysAllZerosProbe * testNonzero = dynamic_cast<TestNotAlwaysAllZerosProbe *>(originalMovieProbe);
   assert(testNonzero->nonzeroValueHasOccurred());

   return PV_SUCCESS;
}

