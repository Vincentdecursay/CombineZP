#pragma once


// CCameraSettings dialog

class CCameraSettings : public CDialog
{
	DECLARE_DYNAMIC(CCameraSettings)

public:
	CCameraSettings(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCameraSettings();

// Dialog Data
	enum { IDD = IDD_CAMSETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int brightnessmin;
	int brightness;
	int brightnessmax;
	int brightnessauto;
	int contrastmin;
	int contrast;
	int contrastmax;
	int contrastauto;
	int huemin;
	int hue;
	int huemax;
	int hueauto;
	int saturationmin;
	int saturation;
	int saturationmax;
	int saturationauto;
	int sharpnessmin;
	int sharpness;
	int sharpnessmax;
	int sharpnessauto;
	int gammamin;
	int gamma;
	int gammamax;
	int gammaauto;
	int colourmin;
	int colour;
	int colourmax;
	int colourauto;
	int whitebalmin;
	int whitebal;
	int whitebalmax;
	int whitebalauto;
	int blcompmin;
	int blcomp;
	int blcompmax;
	int blcompauto;
	int gainmin;
	int gain;
	int gainmax;
	int gainauto;
	afx_msg void OnBnClickedOk2();
	void getsettings(void);
	afx_msg void OnBnClickedOk();
	CString m_CamName;
};
