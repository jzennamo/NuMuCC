# basic source code CMakeLists.txt
#include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

art_make( LIBRARY_NAME ub_numuCCInclusiveFilter
	LIB_LIBRARIES 
		     lardata_RecoBase
		     lardata_AnalysisBase
		     larcore_Geometry
		     larcore_Geometry_Geometry_service
		     lardata_Utilities
		     ${ART_PERSISTENCY_COMMON}
		     ${ART_PERSISTENCY_PROVENANCE}
		     ${ART_FRAMEWORK_CORE}
		     ${ART_FRAMEWORK_PRINCIPAL}
		     ${ART_FRAMEWORK_SERVICES_REGISTRY}
		     ${ART_FRAMEWORK_SERVICES_OPTIONAL}
		     ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
		     ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILEDIRECTORY_SERVICE}
		     ${ART_UTILITIES}
		     ${MF_MESSAGELOGGER}
		     ${MF_UTILITIES}
		     ${FHICLCPP}
		     ${CETLIB}
		     ${ROOT_BASIC_LIB_LIST}
		     ${Boost_SYSTEM_LIBRARY}
	MODULE_LIBRARIES
		     #ub_numuCCInclusiveFilter
		     lardata_RecoBase
		     lardata_AnalysisBase
		     larcore_Geometry
		     larcore_Geometry_Geometry_service
		     lardata_Utilities
		     ${ART_PERSISTENCY_COMMON}
		     ${ART_PERSISTENCY_PROVENANCE}
		     ${ART_FRAMEWORK_CORE}
		     ${ART_FRAMEWORK_PRINCIPAL}
		     ${ART_FRAMEWORK_SERVICES_REGISTRY}
		     ${ART_FRAMEWORK_SERVICES_OPTIONAL}
		     ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
		     ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILEDIRECTORY_SERVICE}
		     ${ART_UTILITIES}
		     ${MF_MESSAGELOGGER}
		     ${MF_UTILITIES}
		     ${FHICLCPP}
		     ${CETLIB}
		     ${ROOT_BASIC_LIB_LIST}
		     ${Boost_SYSTEM_LIBRARY}
)											   

install_headers()
install_source()
install_fhicl()

# Respect the order specified in lar_build, which is:
#
#     "SimpleTypesAndConstants",
#     "larcore_Geometry",
#     "larcore_SummaryData",
#     "lardata_RawData",
#     "lardata_RecoBase",
#     "lardata_OpticalDetectorData",
#     "lardata_AnalysisBase",
#     "lardata_Utilities",
#     "larsim_Simulation",
#     "larsim_MCCheater",
#     "lardata_AnalysisAlg",
#     "lardata_RecoObjects",
#     "larreco_RecoAlg",
#     "larsim_LArG4",
#     "larsim_PhotonPropagation",
#     "larsim_DetSim",
#     "larsim_EventGenerator",
#     "larevt_Filters",
#     "CalData",
#     "larreco_ClusterFinder",
#     "larreco_Genfit",
#     "larreco_HitFinder",
#     "larreco_TrackFinder",
#     "larreco_VertexFinder",
#     "larreco_EventFinder",
#     "larsim_TriggerAlgo",
#     "larana_OpticalDetector",
#     "larana_Calorimetry",
#     "larreco_ShowerFinder",
#     "lareventdisplay_EventDisplay",
#     "larana_ParticleIdentification",
