#include "stdafx.h"

class PicMat;

class Layer{
private:
	CDC _DC;//ֻ�е�����ʱ��ʹ�� ƽʱû��
	CBitmap layerbk, * _fall;
	bool zero;//������û�б����룿
	bool tzero;//����CBitmap�Ƿ���������

	unsigned int _width;//������С
	unsigned int _height;//������С

	unsigned char * _fp;//ֻ�е�����ʱ��ʹ�� ƽʱû�� ����Ҫ�ͷ��ڴ�
	
	int cursor;//��ǰ���ڻ���
	
	struct layer_unit{
		layer_unit * _former;
		layer_unit * _latter;
		bool _visible;
		char  name[180];
		PicMat * _con;
	};

	layer_unit * _fin;//��λ��
	layer_unit * _fcur;//��ǰλ��
	

public:
	Layer();
	~Layer();
	//�����ĳ�Ա����
	bool Load_Pic(char * apath,CBitmap * cbitmap, UINT ut=IMAGE_BITMAP);//��ȡͼƬ ����CBitmap��Ϊ��ʽ
	//��������
	//void Match(void);//��DC��bitmapƥ�� û�б�Ҫƥ�䣬CBitmapֻ����Ϊһ�ű��������õ�
	//���ñ���ͼ��
	bool LoadBackGround(char * apath, UINT ut=IMAGE_BITMAP);//��ȡͼƬ ����CBitmap��Ϊ��ʽ
	void CreateBackGround(unsigned int width,unsigned int height,COLORREF color=0);
	//������ͼ��
	void Newlayer(char * c);//bool?
	void Copylayer(char * c);//���Ƶ�ǰͼ�㵽����
	bool Delete(void);//ɾ����ǰͼ��
	//��õ�ǰ��ͼ��ָ��
	PicMat * Current(void);
	//�ı�ͼ��λ��
	void Top(void);
	void Bottom(void);
	bool Find(char * c);
	void Visible(bool real);//�ı������


	//��������Ҫ�Ļ������Ҹ��Ƶ�CPaintDC��
	void Draw(CPaintDC * dc,CDialog * dia);
	void Draw(CDC * pDC,CView *  cv);
	

};

class PicMat{
	friend class Layer;
private:
	CPoint _bias;
	unsigned int _width;//ͼ��������
	unsigned int _height;//ͼ��������
	unsigned char * _fp;//�ĸ�һ��Ϊһ����λ��������
	bool zero;

public:
	CPoint Bias(void);
	unsigned int Width(void);
	unsigned int Height(void);
	//
	PicMat(CPoint cp,unsigned int width,unsigned int height);
	PicMat();
	~PicMat();
	void operator=(const PicMat & one);
	void NewMat(CPoint cp=CPoint(50,50),unsigned int width=50,unsigned int height=50,COLORREF color=0);
	void Moveto(CPoint cp);//�ƶ���ĳλ��
	void Move(CPoint cp);//�ƶ�����
	bool LoadPic(CPoint cp,char * apath,UINT ut=IMAGE_BITMAP);//����Bitmap ���Զ������·����
	void Transparent(COLORREF color=0);//���ض���ɫת����͸��
	void SetTransparent(unsigned char alpha,bool force=false);//͸����0 - 255 ��͸�� - ��ȫ͸�� force ǿ�Ƹı���ȫ͸������

	//Function ����
	void Inverse();//��ɫ
	void Brightness(float rate=1);//����
	void Saturation(float rate=1);//���Ͷ�
	void Phase(float alpha=0);//��ɫ��λ
	void Rotate(float alpha,bool inter=true);//��ת�任,inter=true ִ���Ƽ���˫��ֵ�任
	void Stretch(unsigned int x,unsigned int y,bool inter=true);//�����任,inter=true ִ���Ƽ���˫��ֵ�任
	void Stretch(double ratex,double ratey,bool inter=true);//�����任��inter=true ִ���Ƽ���˫��ֵ�任
	void Mirror(bool x,bool y);//���淴�� ����x�᾵�淴�� ����y�᾵�淴��
	void Affine(CPoint right_top,CPoint right_bottom,bool inter=true);//����任��inter=true ִ���Ƽ���˫��ֵ�任
	void Perspective(CPoint left_top,CPoint left_bottom,CPoint right_top,CPoint right_bottom,bool inter=true);
	//͸�ӱ任 �������ܵ��ĸ������� inter=true ִ���Ƽ���˫��ֵ�任
	void Cut(CPoint center,unsigned int width,unsigned int height);//�ü�
	
private:
	void RGB2HSI(unsigned char r,unsigned char g,unsigned char b,float &h,float &s,float &i);
	void HSI2RGB(float h,float s,float i,unsigned char &r,unsigned char &g,unsigned char &b);
	unsigned int Index(int x,int y,int i);//Ѱַ����
	void Rotate_U(float alpha);//��˫��ֵ����ת�㷨
	void Stretch_U(unsigned int x,unsigned int y);//��˫��ֵ�������任
	void Affine_U(float kx,float ky,float jx,float jy,int W,int H,unsigned char * fp);//����任��inter=true ִ���Ƽ���˫��ֵ�任
	void Quad(float a,float b,float c,float * out,int &n);//��һԪ���η���
	void Perspective_A(CPoint left_top,CPoint left_bottom,CPoint right_top,CPoint right_bottom);
	void Perspective_U(CPoint left_top,CPoint left_bottom,CPoint right_top,CPoint right_bottom);








};
//cb.CreateBitmap(10,10,1,32,ssss); 1 λƽ�� 32λ
//0 ��ʾ��͸��������