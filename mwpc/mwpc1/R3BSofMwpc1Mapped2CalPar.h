// ----------------------------------------------------------------------
// -----		                R3BSofMwpc1Mapped2CalPar 			          -----
// -----          Created 15/10/19  by G. García Jiménez            -----
// -----  by modifying J.L. Rodriguez-Sanchez  classes for Mwpc2    -----
// ----------------------------------------------------------------------

#ifndef R3BSofMwpc1Mapped2CalPar_H
#define R3BSofMwpc1Mapped2CalPar_H

#include "FairTask.h"
#include "TH1F.h"

class TClonesArray;
class R3BSofMwpc1CalPar;
class R3BEventHeader;

class R3BSofMwpc1Mapped2CalPar : public FairTask
{

  public:
    /** Default constructor **/
    R3BSofMwpc1Mapped2CalPar();

    /** Standard constructor **/
    R3BSofMwpc1Mapped2CalPar(const char* name, Int_t iVerbose = 1);

    /** Destructor **/
    virtual ~R3BSofMwpc1Mapped2CalPar();

    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    /** Virtual method FinishEvent **/
    virtual void FinishEvent();

    /** Virtual method FinishTask **/
    virtual void FinishTask();

    /** Virtual method Reset **/
    virtual void Reset();

    /** Virtual method ReInit **/
    virtual InitStatus ReInit();

    /** Virtual method Search pedestals **/
    virtual void SearchPedestals();

    /** Accessor functions **/
    const Int_t GetNumPadsX() { return fNumPadX; }
    const Int_t GetNumPadsY() { return fNumPadY; }
    const Int_t GetNumParametersFit() { return fNumParams; }
    const Int_t GetCalRange_left() { return fMapHistos_left; }
    const Int_t GetCalRange_right() { return fMapHistos_right; }
    const Int_t GetCalRange_bins() { return fMapHistos_bins; }
    const Int_t GetMinStadistics() { return fMinStadistics; }

    void SetNumPadsX(Int_t numberPadsX) { fNumPadX = numberPadsX; }
    void SetNumPadsY(Int_t numberPadsY) { fNumPadY = numberPadsY; }
    void SetNumParametersFit(Int_t numberParams) { fNumParams = numberParams; }
    void SetCalRange_left(Int_t Histos_left) { fMapHistos_left = Histos_left; }
    void SetCalRange_right(Int_t Histos_right) { fMapHistos_right = Histos_right; }
    void SetCalRange_bins(Int_t Histos_bins) { fMapHistos_bins = Histos_bins; }
    void SetMinStadistics(Int_t minstad) { fMinStadistics = minstad; }

  protected:
    // Number of histograms, limits and bining
    Int_t fNumPadX;
    Int_t fNumPadY;
    Int_t fNumParams;
    Int_t fMapHistos_left;
    Int_t fMapHistos_right;
    Int_t fMapHistos_bins;

    // Minimum stadistics and parameters
    Int_t fMinStadistics;

    R3BSofMwpc1CalPar* fPad_Par;     /**< Parameter container. >*/
    TClonesArray* fMwpcMappedDataCA; /**< Array with Mapped-input data. >*/

    TH1F** fh_Map_q_pad;

  public:
    ClassDef(R3BSofMwpc1Mapped2CalPar, 0);
};

#endif
