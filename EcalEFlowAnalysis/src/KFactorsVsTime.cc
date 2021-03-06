#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <TFile.h>
#include <TTree.h>
#include <TMath.h>
#include "interface/KFactorsVsTime.h"
#include <cmath>
using namespace std;

KFactorsVsTime::KFactorsVsTime(const char* file,bool kFactorsPerXtalInEB)
{
  std::cout << "[KFactorsVsTime]::Opening file " << file << std::endl;
  TFile* kFactors = TFile::Open(file);
  TTree* kFactorsTree= (TTree*) kFactors->Get("kfactors_vsTime");
  std::cout << "[KFactorsVsTime]::Got TTree " << kFactorsTree << std::endl;

  /*
******************************************************************************
  *Tree    :kfactors_vsTime: kfactors_vsTime                                   *
  *Entries :     4880 : Total =          100801 bytes  File  Size =      18582 *
  *        :          : Tree compression factor =   5.48                       *
******************************************************************************
    *Br    0 :start_run : start_run/I                                            *
    *Entries :     4880 : Total  Size=      20097 bytes  File Size  =        429 *
    *Baskets :        1 : Basket Size=      32000 bytes  Compression=  45.70     *
*............................................................................*
    *Br    1 :end_run   : end_run/I                                              *
    *Entries :     4880 : Total  Size=      20087 bytes  File Size  =        431 *
    *Baskets :        1 : Basket Size=      32000 bytes  Compression=  45.49     *
*............................................................................*
    *Br    2 :ieta      : ieta/I                                                 *
    *Entries :     4880 : Total  Size=      20072 bytes  File Size  =        528 *
    *Baskets :        1 : Basket Size=      32000 bytes  Compression=  37.12     *
*............................................................................*
    *Br    3 :det       : det/I                                                  *
    *Entries :     4880 : Total  Size=      20067 bytes  File Size  =        212 *
    *Baskets :        1 : Basket Size=      32000 bytes  Compression=  92.46     *
*............................................................................*
    *Br    4 :kfactor   : kfactor/F                                              *
    *Entries :     4880 : Total  Size=      20087 bytes  File Size  =      16274 *
    *Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.20     *
*............................................................................*
*/

//   for (int i=0;i<85;++i)
//     {
//       kFactorsTimeMap myMap;
//       ebMap_[i]=myMap;
//     }

//   for (int i=0;i<39;++i)
//     {
//       kFactorsTimeMap myMap;
//       eeMap_[i]=myMap;
//     }

  int ieta;
  int iphi;
  int sign;
  int start_run;
  int end_run;
  int det;
  float kf;
  TBranch *b_ring=kFactorsTree->GetBranch("ieta");
  TBranch *b_iphi=kFactorsTree->GetBranch("iphi");
  TBranch *b_sign=kFactorsTree->GetBranch("sign");
  TBranch *b_startrun=kFactorsTree->GetBranch("start_run");
  TBranch *b_endrun=kFactorsTree->GetBranch("end_run");
  TBranch *b_det=kFactorsTree->GetBranch("det");
  TBranch *b_kf=kFactorsTree->GetBranch("kfactor");
  kFactorsTree->SetBranchAddress("ieta", &ieta, &b_ring);
  kFactorsTree->SetBranchAddress("iphi", &iphi, &b_iphi);
  kFactorsTree->SetBranchAddress("sign", &sign, &b_sign);
  kFactorsTree->SetBranchAddress("start_run", &start_run, &b_startrun);
  kFactorsTree->SetBranchAddress("end_run", &end_run, &b_endrun);
  kFactorsTree->SetBranchAddress("det", &det, &b_det);
  kFactorsTree->SetBranchAddress("kfactor", &kf, &b_kf);

  int nentries = kFactorsTree->GetEntries();
  int nentries_eb=0;
  int nentries_ee=0;
  std::cout << "[KFactorsVsTime]::Looping over " << nentries << " entries " << std::endl;
  for(int jentry=0;jentry<nentries;++jentry)
    {
      kFactorsTree->GetEntry(jentry);
      //      std::cout << det << "," << ieta  << "," <<  start_run << "," << end_run << "," << kf << std::endl;
      if (det==0)
	{
	  if (!kFactorsPerXtalInEB)
	    ebMap_[ieta-1][std::make_pair(start_run,end_run)]= kf;
	  else
	    ebXtalMap_[sign][ieta-1][iphi-1][std::make_pair(start_run,end_run)]=kf;
	  nentries_eb++;
	}
      else if (det==1)
	{
	  eeMap_[ieta-1][std::make_pair(start_run,end_run)]= kf;
	  nentries_ee++;
	}
      else
	std::cout << "Don't know what to do..." << std::endl;
    }

  if (kFactorsPerXtalInEB)
    {
      //Assuming to have the same numbers of time intervals per xtal
      int isize=ebXtalMap_[1][1][1].size();
      for (kFactorsTimeMap::const_iterator interval=ebXtalMap_[1][1][1].begin();interval!=ebXtalMap_[1][1][1].end();interval++)
	{
	  for (int iring=0;iring<85;++iring)
	    {
	      int nx=0;
	      float kf=0;
	      for (int iside=0;iside<2;++iside)
		for (int iphi=0;iphi<360;++iphi)
		  {
		    if (ebXtalMap_[iside][iring][iphi].size()!=isize)
		      {
			//			std::cout << "Hello wrong xtal " << iring << "," << iphi << "," << iside << std::endl;
			continue;
		      }
		    nx++;
		    kf+=ebXtalMap_[iside][iring][iphi][interval->first];
		  }
	      if (nx>0)
		{
		  kf=kf/(float)nx;
		  //		  std::cout << iring << "," << kf << std::endl;
		  ebMap_[iring][interval->first]=kf;
		}
	    }
	}
    }
  std::cout << "[KFactorsVsTime]::Found " << nentries_eb <<  " entries in EB " << nentries_ee <<  " entries in EE " << std::endl;
}


float KFactorsVsTime::kFactor(const int& det, const int& ring, const int& run)
{
  kFactorsTimeMap* map=0;
  if (det==0)
    map=&(ebMap_[ring]);
  else if (det==1)
    map=&(eeMap_[ring]);
  else
    return -1;

  if (!map)
    return -1;

  kFactorsTimeMap::const_iterator it=map->begin();
  int minDeltaRun=999999;
  kFactorsTimeMap::const_iterator closest;
  for (it;it!=map->end();++it)
    {
      int deltaRun=min( TMath::Abs( int(run - it->first.first) ), TMath::Abs( int(run- it->first.second) ) );
      if (deltaRun<=minDeltaRun)
	{
	  minDeltaRun=deltaRun;
	  closest=it;
	}
      if (run>=it->first.first && run<=it->first.second)
	return it->second;
    }
  return closest->second;
}

float KFactorsVsTime::kFactor(const int& det, const int& sign, const int& ieta, const int& iphi, const int& run)
{
  if (det==1)
    return kFactor(det,ieta,run);
  
  kFactorsTimeMap* map=0;
  map=&(ebXtalMap_[sign][ieta][iphi]);

  if (!map)
    return -1;

  if (map->size()==0)
    return -1;

  kFactorsTimeMap::const_iterator it=map->begin();
  int minDeltaRun=999999;
  kFactorsTimeMap::const_iterator closest;
  for (it;it!=map->end();++it)
    {
      int deltaRun=min( TMath::Abs( int(run - it->first.first) ), TMath::Abs( int(run- it->first.second) ) );
      if (deltaRun<=minDeltaRun)
	{
	  minDeltaRun=deltaRun;
	  closest=it;
	}
      if (run>=it->first.first && run<=it->first.second)
	return it->second;
    }
  return closest->second;
}
