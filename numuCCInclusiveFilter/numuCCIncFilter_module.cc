////////////////////////////////////////////////////////////////////////
// Class:       numuCCIncFilter
// Module Type: filter
// File:        numuCCIncFilter_module.cc
//
// Version by Lorena and John 
// from cetpkgsupport v1_10_01.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/FindManyP.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "lardata/Utilities/AssociationUtil.h"

#include <memory>

#include "art/Framework/Services/Optional/TFileService.h"

#include "larcore/CoreUtils/ServiceUtil.h"
#include "larcore/Geometry/Geometry.h"
#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"
#include "larcore/Geometry/PlaneGeo.h"
#include "larcore/Geometry/WireGeo.h"
#include "lardata/Utilities/AssociationUtil.h"

#include "lardata/RecoBase/Hit.h"
#include "lardata/RecoBase/Track.h"
#include "lardata/RecoBase/Vertex.h"
#include "lardata/RecoBase/OpFlash.h"
#include "lardata/AnalysisBase/CosmicTag.h"
#include "lardata/AnalysisBase/FlashMatch.h"
#include "lardata/AnalysisBase/T0.h"


class numuCCIncFilter;

class numuCCIncFilter : public art::EDFilter {
public:
  explicit numuCCIncFilter(fhicl::ParameterSet const & pset);
  // The destructor generated by the compiler is fine for classes
  // without bare pointers or other resource use.
  
  // Plugins should not be copied or assigned.
  numuCCIncFilter(numuCCIncFilter const &) = delete;
  numuCCIncFilter(numuCCIncFilter &&) = delete;
  numuCCIncFilter & operator = (numuCCIncFilter const &) = delete;
  numuCCIncFilter & operator = (numuCCIncFilter &&) = delete;
  
  // Required functions.
  bool filter(art::Event & event) override;
  
  // Selected optional functions.
  void reconfigure(fhicl::ParameterSet const & pset) override;
  
  bool inFV(Double_t x, Double_t y, Double_t z) const;

  double FlashTrackDist(double& flash, double start, double end) const;

private:

  std::string                fTrackModuleLabel;        ///< Producer of input tracks
  std::string                fVertexModuleLabel;       ///< Producer of input vertices
  std::string                fCosmicModuleLabel;       ///< Producer of cosmic track tags
  std::string                fOpFlashModuleLabel;      ///< Producer of flashes
  double                     fCosmicScoreCut;          ///< Cut value for possible cosmic tag scores
  double                     fNeutrinoVtxTrackDistCut; ///< Cut to select neutrino candidate
  
};


double flash_time[1000];      
double flash_pe[1000];         
double flash_ycenter[1000];    
double flash_zcenter[1000];    
double flash_ywidth[1000];  
double flash_zwidth[1000];  
double flash_timewidth[1000];

numuCCIncFilter::numuCCIncFilter(fhicl::ParameterSet const & pset)
{
    this->reconfigure(pset);
    
    art::ServiceHandle<art::TFileService> tfs;

    produces< art::Assns<recob::Vertex, recob::Track> >();

}

//------------------------------------------------------------------------------------------------------------------------------------------
bool numuCCIncFilter::inFV(Double_t x, Double_t y, Double_t z) const {
      art::ServiceHandle<geo::Geometry> geom;      
      //set borders of the FV
      double xmin=10.;
      double xmax=2.*geom->DetHalfWidth()-10.;
      double ymin=-geom->DetHalfHeight()+20.;
      double ymax=geom->DetHalfHeight()-20.;
      double zmin=10.;
      double zmax=geom->DetLength()-10.;

      if((x < xmax) && (x > xmin) && (y < ymax) && (y > ymin) && (z < zmax) && (z > zmin)) return true;
      else return false;
}

//This function returns the distance between a flash and 
//a track (in one dimension, here used only for z direction)
double numuCCIncFilter::FlashTrackDist(double& flash, double start, double end) const {
      if(end >= start) {
        if(flash < end && flash > start) return 0;
        else return TMath::Min(fabs(flash-start), fabs(flash-end));
      }
      else {
        if(flash > end && flash < start) return 0;
        else return TMath::Min(fabs(flash-start), fabs(flash-end));
      }
}



bool numuCCIncFilter::filter(art::Event & event)
{
  bool pass = false;

  std::cout << " Let's get this show on the road! " << std::endl; 

    art::ServiceHandle<geo::Geometry> geom;


    // Agreed convention is to ALWAYS output to the event store so get a pointer to our collection
    std::unique_ptr<std::vector<recob::Track> > neutrinoTracks(new std::vector<recob::Track>);
    std::unique_ptr<art::Assns<recob::Vertex, recob::Track> > vertexTrackAssociations(new art::Assns<recob::Vertex, recob::Track>);
    
   
    // Recover the hanles to the vertex and track collections we want to analyze.
    art::Handle< std::vector<recob::Vertex> >  vertexVecHandle;
    art::Handle< std::vector<recob::Track> >   trackVecHandle;
    art::Handle< std::vector<recob::OpFlash> > flashListHandle;
    //    art::Handle< std::vector<anab::T0> >       t0Handle;

    event.getByLabel(fVertexModuleLabel,    vertexVecHandle);
    event.getByLabel(fTrackModuleLabel,     trackVecHandle);

    //----------------------------------------------------
    std::vector<art::Ptr<recob::OpFlash> > flashlist;    

    //NFlashes-->total number of flashes in each event

    //int kMaxFlashes=1000;
    if (event.getByLabel(fOpFlashModuleLabel,flashListHandle))
      art::fill_ptr_vector(flashlist, flashListHandle);
    
    const int NFlashes = flashlist.size();
    
    std::cout << "Interating through " << NFlashes << " flashes..." << std::endl;  

    for (int i = 0; i < NFlashes; ++i){//loop over hits
 
      //for (size_t i = 0; i < NFlashes && i < kMaxFlashes ; ++i){//loop over hits
      flash_time[i]       = flashlist[i]->Time();
      flash_pe[i]         = flashlist[i]->TotalPE();
      flash_ycenter[i]    = flashlist[i]->YCenter();
      flash_zcenter[i]    = flashlist[i]->ZCenter();
      flash_ywidth[i]     = flashlist[i]->YWidth();
      flash_zwidth[i]     = flashlist[i]->ZWidth();
      flash_timewidth[i]  = flashlist[i]->TimeWidth();
    }

    std::cout << "All done!" << std::endl; 

    // Require valid handles, otherwise nothing to do
    // means?
    if (vertexVecHandle.isValid() && vertexVecHandle->size() > 0 && trackVecHandle.isValid() && trackVecHandle->size() > 0)
    {

      std::cout << " Aw, man we got some vertices and tracks...now I am getting excited!! " << std::endl; 
        // Recover associations relating cosmic tags and track
        art::FindManyP<anab::CosmicTag> cosmicAssns(trackVecHandle, event, fCosmicModuleLabel);

        // We need to keep track of the best combination
        // Can we assign art ptrs? I don't think so...
        std::vector<art::Ptr<recob::Vertex>> bestVertexVec;
        std::vector<art::Ptr<recob::Track> > bestTrackVec;
        double bestDistance(100000.);
       

        //define cut variables
        double flashwidth = 80; //cm. Distance flash-track
        //double distcut = 5; //cm. Distance track start/end to vertex
        //double lengthcut = 75; //cm. Length of longest track
        double beammin = 3.55/*-0.36*/; //us. Beam window start
        double beammax = 5.15/*-0.36*/; //us. Beam window end
        double PEthresh = 50; //PE
                 
        double trklen_longest=0;      
        //double trktheta_longest=0;
        //int longtracktag=0;
        //int trackcandidate=0;
	// int vertexcandidate=0;
        double flashmax=0;
        int theflash=-1;
        bool flashtag=false;
 
        std::cout<<"Start looping over " <<  NFlashes << " flashes and get the maximum and flash number"<<std::endl; 

        //----loop over all the flashes and check if there are flashes within the beam
        //window and above the PE threshold
        for(int f=0; f < NFlashes; f++){ 
        //for(int f=0; f<NFlashes && f < kMaxFlashes; f++){ 
          //flash time within the beam window????
          if((flash_time[f]> beammin && flash_time[f]<beammax)&&flash_pe[f]>PEthresh) {
               flashtag=true;
               //get the flash index corresponding to the maximum PE
               if(flash_pe[f]>flashmax)
               {
                   theflash=f;
                   flashmax=flash_pe[f];
               }
          }          
        }  //end of loop over all the flashes f
        
	std::cout << "all done, we know that flashtah==" << flashtag << std::endl; 

        //
        bool flashmatchtag=false;
        //----------------------------------------------------
        // Outer loop is over the vertices in the collection

	std::cout << "gonna iterate through " << vertexVecHandle->size() << "verticies" << std::endl; 
	
        for(size_t vertexIdx = 0; vertexIdx < vertexVecHandle->size(); vertexIdx++)
        {
            // Recover art ptr to vertex
            art::Ptr<recob::Vertex> vertex(vertexVecHandle, vertexIdx);
            
            // Get the position of the vertex
            // Ultimately we really want the vertex position in a TVector3 object...
            double vertexXYZ[3];
            
            vertex->XYZ(vertexXYZ);
            
            TVector3 vertexPos(vertexXYZ[0],vertexXYZ[1],vertexXYZ[2]);
	    
            //check if the vertex is within the FV;
            if(!inFV(vertexPos.X(),vertexPos.Y(),vertexPos.Z())) continue;
	            
            // For each vertex we loop over all tracks looking for matching pairs
            // The outer loop here, then is over one less than all tracks
	    std::cout << "Gonna Loop over " << trackVecHandle->size() << "tracks" << std::endl; 
            for(size_t track1Idx = 0; track1Idx < trackVecHandle->size(); track1Idx++)
            {
                // Work with an art Ptr here
                art::Ptr<recob::Track> track1(trackVecHandle,track1Idx);
                
                // Is there a CosmicTag associated with this track?
                // There are other/better ways to handle this but I think this covers worst case scenario
                if (cosmicAssns.isValid() && cosmicAssns.size() > 0)
                {
                    std::vector<art::Ptr<anab::CosmicTag> > cosmicVec = cosmicAssns.at(track1.key());
                    
                    if (!cosmicVec.empty())
                    {
                        art::Ptr<anab::CosmicTag>& cosmicTag(cosmicVec.front());
                        
                        if (cosmicTag->CosmicScore() > fCosmicScoreCut) continue;
                    }
                }
                
                // Currently we have the problem that tracks can be fit in the "wrong" direction
                // so we need to get the track direction sorted out.
                TVector3 track1Pos = track1->Vertex();
                TVector3 track1End = track1->End();

                //declare track length/track angle  will be used for the track selection
                //double trklen = track.Length();
                //double trktheta  = track1.Theta();
                // select the trklen and trktheta               
 
                // Take the closer end---------------------------------
                double track1ToVertexDist = (track1Pos - vertexPos).Mag();
                
                if ((track1End - vertexPos).Mag() < track1ToVertexDist)
                {
                    track1Pos          = track1->End();
                    track1End          = track1->Vertex();
                    track1ToVertexDist = (track1Pos - vertexPos).Mag();
                }
                // calculate the cos angle of track1
                double trktheta=fabs(track1End.z()-track1Pos.z())/(track1End-track1Pos).Mag();

                //-------------------------------------------------------
                //get the length of the longest forward going track and close to vertex
                if((track1End-track1Pos).Mag()> trklen_longest && trktheta>0.85 && track1ToVertexDist<5) {
                  trklen_longest=(track1End-track1Pos).Mag();
                  //trackcandidate=track1Idx;
		  //                  vertexcandidate=vertexIdx;
                  //check if the longest track if flash matched.
                  if (FlashTrackDist(flash_zcenter[theflash], track1Pos.z(), track1End.z() )< flashwidth ) {flashmatchtag=true;}

                }  
                // Is there a cut at this point?
	    }  //end of loop over the first track
        } // loop over the vertex
       
	// Check to see if we think we have a candidate
        if (bestDistance < fNeutrinoVtxTrackDistCut && trklen_longest > 75 && flashmatchtag == true)
        {
            // Make an association between the best vertex and the matching tracks
            util::CreateAssn(*this, event, bestVertexVec[0], bestTrackVec, *vertexTrackAssociations);
	    
	    pass = true;

        }
    }

    std::cout << "Done!!!!!! With a vertex" << std::endl; 
    // Add tracks and associations to event.
    event.put(std::move(vertexTrackAssociations));
      
    return pass;
}//end filter 

void numuCCIncFilter::reconfigure(fhicl::ParameterSet const & pset)
{
  
  fTrackModuleLabel        = pset.get<std::string> ("TrackModuleLabel");
  fVertexModuleLabel       = pset.get<std::string> ("VertexModuleLabel");    
  fCosmicModuleLabel       = pset.get<std::string> ("CosmicModuleLabel");
  fOpFlashModuleLabel      = pset.get<std::string> ("OpFlashModuleLabel");
  fCosmicScoreCut          = pset.get<double>      ("CosmicScoreCut");
  fNeutrinoVtxTrackDistCut = pset.get<double>      ("NuVtxTrackDistCut");
    
}

DEFINE_ART_MODULE(numuCCIncFilter)




	  



