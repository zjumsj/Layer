#include "stdafx.h"
#include "Layer.h"
#include "string.h"
#include "math.h"

Layer::~Layer(){
	if (!tzero)
		delete _fall;
	layer_unit * temp;
	temp=_fin;
	if (temp==NULL) return;
	while(1){
		delete temp->_con;
		if (temp->_latter==NULL){
			delete temp;
			break;
		}
		else{
			temp=temp->_latter;	
			delete temp->_former;
		}
	}
}

bool Layer::Load_Pic(char * apath,CBitmap * cbitmap,UINT ut){//cbitmap ������� IMAGE_BITMAP IMAGE_CURSOR IMAGE_ICON
	HBITMAP hBitmap;
	hBitmap=(HBITMAP)::LoadImage(NULL,apath,ut,0,0,LR_LOADFROMFILE);
	if (!hBitmap)
		return false;
	else{
		cbitmap->Attach(hBitmap);
		return true;
	}
}


Layer::Layer()
:zero(true),tzero(true),cursor(-1),_fin(NULL),_fcur(NULL)
{
	//_DC.CreateCompatibleDC(dc);//�ڽ�һ��ƥ��DC
	//_DC.CreateDC(TEXT("DISPLAY"),NULL,NULL,NULL);
}

void Layer::Visible(bool real){if (cursor<0) return; _fcur->_visible=real; return;}


bool Layer::LoadBackGround(char * apath,UINT ut){
	
	if(!zero)
		layerbk.DeleteObject();

	HBITMAP hBitmap;
	hBitmap=(HBITMAP)::LoadImage(NULL,apath,ut,0,0,LR_LOADFROMFILE);
	
	if (!hBitmap)
		return false;
	else{

		BITMAP bm;

		layerbk.Attach(hBitmap);
		layerbk.GetBitmap(&bm);
		_width=bm.bmWidth;
		_height=bm.bmHeight;
		//
		 //FP δ��ʼ
		//
		zero=false;
		return true;
	}	
}

/*
void Layer::Match(void){
	_DC.SelectObject(&layerbk);//��BackGroudû������֮ǰʹ�û����
}
*/

void Layer::CreateBackGround(unsigned int width,unsigned int height,COLORREF color){

	if (width<1 || height<1)
		return;
	if (!zero)
		layerbk.DeleteObject();

	int i;
	_width=width;
	_height=height;
	DWORD * memory=new DWORD[width*height];
	for (i=0;i<width*height;i++)
		memory[i]=color;

	layerbk.CreateBitmap((int)width,(int)height,1,32,memory);
	delete [] memory;
	zero=false;
}

void Layer::Newlayer(char * c){
	layer_unit * newlayer=new layer_unit;
	//��ͼ����Զ�ö�
	if (_fin) _fin->_former=newlayer;
	newlayer->_former=NULL;
	newlayer->_latter=_fin;
	newlayer->_visible=true;
	_fin=newlayer;
	_fcur=_fin;

	cursor=0;
	strcpy(newlayer->name,c);//��������
	newlayer->_con=new PicMat;
}
void Layer::Copylayer(char * c){
	layer_unit * newlayer=new layer_unit;
	layer_unit * formerlayer;
	formerlayer=_fcur;//���浱ǰlayerλ��
	//��ͼ����Զ�ö�
	if (_fin) _fin->_former=newlayer;
	newlayer->_former=NULL;
	newlayer->_latter=_fin;
	newlayer->_visible=true;
	_fin=newlayer;
	_fcur=_fin;

	cursor=0;
	strcpy(newlayer->name,c);//��������
	//��ֵ
	newlayer->_con=new PicMat;
	(*(newlayer->_con))=(*(formerlayer->_con));
}
bool Layer::Delete(void){
	struct layer_unit * lu;
	
	if(cursor<0)//������
		return false;
	if(cursor==0){//ɾ����ͼ��
		lu=_fin;
		if(_fin->_latter!=NULL){
			_fin=_fin->_latter;
			(_fin->_latter)->_former=NULL;
			cursor=0;
			_fcur=_fin;
		}
		else{
			_fin=NULL;
			cursor=-1;
			_fcur=NULL;
			//zero=true; //����������
		}
	}
	else{
		lu=_fcur;
		(_fcur->_former)->_latter=_fcur->_latter;
		if (_fcur->_latter!=NULL)
			(_fcur->_latter)->_former=_fcur->_former;
		cursor=0;
		_fcur=_fin;
	}
	delete lu->_con;
	delete lu;
	return true;
}
PicMat * Layer::Current(void){
	return _fcur->_con;
}
//�ö�
void Layer::Top(void){
	if (cursor<0) 
		return;
	if (_fin==_fcur)//�Ѿ��ö�
		return;

	//�����Ƴ�λ��
	(_fcur->_former)->_latter=_fcur->_latter;
	if (_fcur->_latter!=NULL)
		(_fcur->_latter)->_former=_fcur->_former;
	//�����Ƴ���
	_fcur->_former=NULL;
	_fcur->_latter=_fin;
	//��������λ��
	_fin->_former=_fcur;
	_fin=_fcur;
	cursor=0;
}
//�õײ�
void Layer::Bottom(void){
	if (cursor<0)
		return;
	int btt=cursor;
	layer_unit * temp;

	temp=_fcur;
	while (temp->_latter!=NULL){
		btt++;
		temp=temp->_latter;
	}
	if (temp==_fcur)
		return;//�Ѿ��õ�
	
	//�����Ƴ�λ��ǰ�� ������BUG
	if(_fcur->_former!=NULL)
		(_fcur->_former)->_latter=_fcur->_latter;
	else
		_fin=_fcur->_latter;//�����׵�ַָ��
	(_fcur->_latter)->_former=_fcur->_former;
	//��������λ��
	temp->_latter=_fcur;
	//�����Ƴ���
	_fcur->_latter=NULL;
	_fcur->_former=temp;
	
	cursor=btt;
}

bool Layer::Find(char * c){
	if (cursor<0)
		return false;
	int btt;
	layer_unit * temp;
	
	btt=0;
	temp=_fin;
	while (1){
		if (strcmp(temp->name,c)==0)
		{
			_fcur=temp;
			cursor=btt;
			return true;
		}
		btt++;
		if (temp->_latter!=NULL)
			temp=temp->_latter;
		else
			break;
	}
	return false;
}
void Layer::Draw(CDC * pDC,CView * cv){
	if (zero) return;//ȱʧ����
	_fp=new unsigned char[4*_width*_height];//�� �� һ�����ص�4��Byte

	/*α����
	��ͼƬ��Ϣ�þ����ʾ����
	�ӵײ㿪ʼ
	��͸�����㷨���е���
	�����ӽ��CBitmap
	CBitmap��select
	���Ƶ��ⲿCPaitDC
	*/
	
	//��ȡ����
	BITMAP bm;

	LONG bmWidth;
	LONG bmHeight;
	LONG bmWidthBytes;

	int i,j,k;

	unsigned char * memory;//����һ���ڴ�ռ�
	
	unsigned char * fp;
	unsigned char  binary;
	unsigned char flag;
	int a,b;

	layerbk.GetBitmap(&bm);
	
	bmWidth=bm.bmWidth;
	bmHeight=bm.bmHeight;
	bmWidthBytes=bm.bmWidthBytes;

	memory=new unsigned char[bmHeight*bmWidthBytes];//����
	layerbk.GetBitmapBits(bmHeight*bmWidthBytes,memory);//��ȡ����


	

	if(bmWidthBytes/bmWidth>=3){
		for (j=0;j<bmHeight;j++){//CBitmap��ͼ���������ͨ�� 16ɫ�ǵ�ͨ����
			fp=memory+j*bmWidthBytes;
			for(i=0;i<bmWidth;i++){
				_fp[j*_width*4+i*4]=fp[i*4];//B
				_fp[j*_width*4+i*4+1]=fp[i*4+1];//G
				_fp[j*_width*4+i*4+2]=fp[i*4+2];//R
				_fp[j*_width*4+i*4+3]=0;//A ����Ϊ��͸����
			}
		}
	}
	else	
	{
		for (j=0;j<bmHeight;j++){//CBitmap�����������ֵ�õ���
			fp=memory+j*bmWidthBytes;
			a=0;b=0;
			for(i=0;i<bmWidth;i++){//˳���д���֤
				binary=fp[a];
				flag=0;
				switch(b){//���ݴ洢˳�� ��λ����λ
				case 0: if (binary & 0x80) flag=1;break;
				case 1: if (binary & 0x40) flag=1;break;
				case 2: if (binary & 0x20) flag=1;break;
				case 3: if (binary & 0x10) flag=1;break;
				case 4: if (binary & 0x08) flag=1;break;
				case 5: if (binary & 0x04) flag=1;break;
				case 6: if (binary & 0x02) flag=1;break;
				case 7: if (binary & 0x01) flag=1;break;				
				}
				_fp[j*_width*4+4*i]=flag*255;
				_fp[j*_width*4+4*i+1]=flag*255;
				_fp[j*_width*4+4*i+2]=flag*255;
				_fp[j*_width*4+4*i+3]=0;//����Ϊ��͸����
				b++;
				if (b==8){
					b=0;
					a++;
				}
			}
		}
	}
	delete [] memory;

	//�ӵײ㿪ʼ����
	PicMat * apicmat;
	CRect acrect;
	int intt;
	int xx,yy;//������ͼ���ƫ��
	unsigned int innerwidth;//������ͼ����
	int lp1;
	int lp2;
	double p;//Ȩ


	if (cursor>-1){
		int btt=cursor;
		layer_unit * temp=_fcur;
		while (temp->_latter!=NULL){
			btt++;
			temp=temp->_latter;
		}
		//��ʼ����
		while (1){
			if (temp->_visible){//�ɼ�
				apicmat=temp->_con;
				innerwidth=2*apicmat->_width;

				//Left
				intt=(apicmat->_bias).x-apicmat->_width;
				if (intt>0){
					acrect.left=intt;
					xx=0;
				}
				else{
					acrect.left=0;
					xx=0-intt;
				}

				//Top
				intt=(apicmat->_bias).y-apicmat->_height;
				if (intt>0){
					acrect.top=intt;
					yy=0;
				}
				else{
					acrect.top=0;
					yy=0-intt;
				}
				
				//Right
				intt=(apicmat->_bias).x+apicmat->_width;
				if (intt<_width)
					acrect.right=intt;
				else
					acrect.right=_width;

				//Bottom
				intt=(apicmat->_bias).y+apicmat->_height;
				if (intt<_height)
					acrect.bottom=intt;
				else
					acrect.bottom=_height;

				for(i=xx,a=acrect.left;a<acrect.right;i++,a++){
					for (j=yy,b=acrect.top;b<acrect.bottom;j++,b++){
						p=apicmat->_fp[4*j*innerwidth+4*i+3];
						lp1=4*b*_width+4*a;
						lp2=4*j*innerwidth+4*i;
						for (k=0;k<3;k++,lp1++,lp2++){
							_fp[lp1]=(unsigned char)((_fp[lp1]*p+(255-p)*apicmat->_fp[lp2])/255.0+0.5);
						}
					}
				}			
			}
			if(temp->_former==NULL)//����ѭ��
				break;
			else
				temp=temp->_former;
		}
	}
	//��ʼƥ��
	CBitmap * _all=new CBitmap;
	CRect acr;

	_all->CreateBitmap(_width,_height,1,32,NULL);
	_all->SetBitmapBits(4*_width*_height,_fp);
	_DC.CreateCompatibleDC(pDC);
	_DC.SelectObject(_all);
    
	
	cv->GetClientRect(&acr);
	pDC->StretchBlt(0,0,acr.Width(),acr.Height(),&_DC,0,0,_width,_height,SRCCOPY);
	_DC.DeleteDC();

	
	if (!tzero)
		delete _fall;
	
	_fall=_all;
	tzero=false;
	delete [] _fp;
}
void Layer::Draw(CPaintDC * dc,CDialog * dia){
	if (zero) return;//ȱʧ����
	_fp=new unsigned char[4*_width*_height];//�� �� һ�����ص�4��Byte

	/*α����
	��ͼƬ��Ϣ�þ����ʾ����
	�ӵײ㿪ʼ
	��͸�����㷨���е���
	�����ӽ��CBitmap
	CBitmap��select
	���Ƶ��ⲿCPaitDC
	*/
	
	//��ȡ����
	BITMAP bm;

	LONG bmWidth;
	LONG bmHeight;
	LONG bmWidthBytes;

	int i,j,k;

	unsigned char * memory;//����һ���ڴ�ռ�
	
	unsigned char * fp;
	unsigned char  binary;
	unsigned char flag;
	int a,b;

	layerbk.GetBitmap(&bm);
	
	bmWidth=bm.bmWidth;
	bmHeight=bm.bmHeight;
	bmWidthBytes=bm.bmWidthBytes;

	memory=new unsigned char[bmHeight*bmWidthBytes];//����
	layerbk.GetBitmapBits(bmHeight*bmWidthBytes,memory);//��ȡ����


	

	if(bmWidthBytes/bmWidth>=3){
		for (j=0;j<bmHeight;j++){//CBitmap��ͼ���������ͨ�� 16ɫ�ǵ�ͨ����
			fp=memory+j*bmWidthBytes;
			for(i=0;i<bmWidth;i++){
				_fp[j*_width*4+i*4]=fp[i*4];//B
				_fp[j*_width*4+i*4+1]=fp[i*4+1];//G
				_fp[j*_width*4+i*4+2]=fp[i*4+2];//R
				_fp[j*_width*4+i*4+3]=0;//A ����Ϊ��͸����
			}
		}
	}
	else	
	{
		for (j=0;j<bmHeight;j++){//CBitmap�����������ֵ�õ���
			fp=memory+j*bmWidthBytes;
			a=0;b=0;
			for(i=0;i<bmWidth;i++){//˳���д���֤
				binary=fp[a];
				flag=0;
				switch(b){//���ݴ洢˳�� ��λ����λ
				case 0: if (binary & 0x80) flag=1;break;
				case 1: if (binary & 0x40) flag=1;break;
				case 2: if (binary & 0x20) flag=1;break;
				case 3: if (binary & 0x10) flag=1;break;
				case 4: if (binary & 0x08) flag=1;break;
				case 5: if (binary & 0x04) flag=1;break;
				case 6: if (binary & 0x02) flag=1;break;
				case 7: if (binary & 0x01) flag=1;break;				
				}
				_fp[j*_width*4+4*i]=flag*255;
				_fp[j*_width*4+4*i+1]=flag*255;
				_fp[j*_width*4+4*i+2]=flag*255;
				_fp[j*_width*4+4*i+3]=0;//����Ϊ��͸����
				b++;
				if (b==8){
					b=0;
					a++;
				}
			}
		}
	}
	delete [] memory;

	//�ӵײ㿪ʼ����
	PicMat * apicmat;
	CRect acrect;
	int intt;
	int xx,yy;//������ͼ���ƫ��
	unsigned int innerwidth;//������ͼ����
	int lp1;
	int lp2;
	double p;//Ȩ


	if (cursor>-1){
		int btt=cursor;
		layer_unit * temp=_fcur;
		while (temp->_latter!=NULL){
			btt++;
			temp=temp->_latter;
		}
		//��ʼ����
		while (1){
			if (temp->_visible){//�ɼ�
				apicmat=temp->_con;
				innerwidth=2*apicmat->_width;

				//Left
				intt=(apicmat->_bias).x-apicmat->_width;
				if (intt>0){
					acrect.left=intt;
					xx=0;
				}
				else{
					acrect.left=0;
					xx=0-intt;
				}

				//Top
				intt=(apicmat->_bias).y-apicmat->_height;
				if (intt>0){
					acrect.top=intt;
					yy=0;
				}
				else{
					acrect.top=0;
					yy=0-intt;
				}
				
				//Right
				intt=(apicmat->_bias).x+apicmat->_width;
				if (intt<_width)
					acrect.right=intt;
				else
					acrect.right=_width;

				//Bottom
				intt=(apicmat->_bias).y+apicmat->_height;
				if (intt<_height)
					acrect.bottom=intt;
				else
					acrect.bottom=_height;

				for(i=xx,a=acrect.left;a<acrect.right;i++,a++){
					for (j=yy,b=acrect.top;b<acrect.bottom;j++,b++){
						p=apicmat->_fp[4*j*innerwidth+4*i+3];
						lp1=4*b*_width+4*a;
						lp2=4*j*innerwidth+4*i;
						for (k=0;k<3;k++,lp1++,lp2++){
							_fp[lp1]=(unsigned char)((_fp[lp1]*p+(255-p)*apicmat->_fp[lp2])/255.0+0.5);
						}
					}
				}			
			}
			if(temp->_former==NULL)//����ѭ��
				break;
			else
				temp=temp->_former;
		}
	}
	//��ʼƥ��
	CBitmap * _all=new CBitmap;
	CRect acr;

	_all->CreateBitmap(_width,_height,1,32,NULL);
	_all->SetBitmapBits(4*_width*_height,_fp);
	_DC.CreateCompatibleDC(dc);
	_DC.SelectObject(_all);
   
	dia->GetWindowRect(&acr);
	dc->StretchBlt(0,0,acr.Width(),acr.Height(),&_DC,0,0,_width,_height,SRCCOPY);
	_DC.DeleteDC();

	
	if (!tzero)
		delete _fall;
	
	_fall=_all;
	tzero=false;
	delete [] _fp;
}
//--------------------PicMat---------------------------
CPoint PicMat::Bias(void){return _bias;}
unsigned int PicMat::Width(void){return _width;}
unsigned int PicMat::Height(void){return _height;}

PicMat::PicMat(CPoint cp,unsigned int width,unsigned int height){
	if (width<1 || height<1){
		_bias.x=_bias.y=0;
		_width=0;
		_height=0;
		_fp=NULL;
		zero=true;
	}
	else{
		_bias=cp;
		_width=width;
		_height=height;
		_fp=new unsigned char[2*width*2*height*4];
		zero=false;
	}
}

PicMat::PicMat(){
	_bias.x=_bias.y=0;
	_width=0;
	_height=0;
	_fp=NULL;
	zero=true;
}
PicMat::~PicMat(){
	if (!zero){
		delete [] _fp;	
	}
}
void PicMat::operator =(const PicMat & one){
	if (!zero)
		delete [] _fp;
	_bias=one._bias;
	_width=one._width;
	_height=one._height;
	zero=one.zero;
	if(!zero){
		_fp=new unsigned char[2*_width*2*_height*4];
		memcpy(_fp,one._fp,2*_width*2*_height*4);
	}
}
bool PicMat::LoadPic(CPoint cp,char * apath,UINT ut){
	int i,j;
	
	CBitmap cbitmap;
	HBITMAP hBitmap;
	BITMAP bitmap;

	LONG bmWidth;
	LONG bmHeight;
	LONG bmWidthBytes;

	unsigned char symbol;
	unsigned char * memory;//����һ���ڴ�ռ�
	
	hBitmap=(HBITMAP)::LoadImage(NULL,apath,ut,0,0,LR_LOADFROMFILE);
	if (!hBitmap)
		return false;
	cbitmap.Attach(hBitmap);
	
	_bias=cp;

	
	cbitmap.GetBitmap(&bitmap);//��ȡBitmap��Ϣ
	bmWidth=bitmap.bmWidth;
	bmHeight=bitmap.bmHeight;
	bmWidthBytes=bitmap.bmWidthBytes;
	
	symbol=0;
	_width=(bmWidth+1)/2;
	symbol=symbol+0x10*(bmWidth%2);//��λ��ʾ��
	_height=(bmHeight+1)/2;
	symbol=symbol+(bmHeight%2);//��λ��ʾ��

	if (!zero)
		delete [] _fp;
	_fp=new unsigned char[2*_width*2*_height*4];//�½��ڴ�

	memory=new unsigned char[bmHeight*bmWidthBytes];//����
	cbitmap.GetBitmapBits(bmHeight*bmWidthBytes,memory);//��ȡ����
	
	//���ݿ���
	unsigned char * fp;
	unsigned char  binary;
	unsigned char flag;
	int a,b;

	if(bmWidthBytes/bmWidth>=3){
		for (j=0;j<bmHeight;j++){//CBitmap��ͼ���������ͨ�� 16ɫ�ǵ�ͨ����
			fp=memory+j*bmWidthBytes;
			for(i=0;i<bmWidth;i++){
				_fp[j*2*_width*4+i*4]=fp[i*4];//B
				_fp[j*2*_width*4+i*4+1]=fp[i*4+1];//G
				_fp[j*2*_width*4+i*4+2]=fp[i*4+2];//R
				_fp[j*2*_width*4+i*4+3]=0;//A ����Ϊ��͸����
			}
			if (symbol & 0x10)
				_fp[(j+1)*2*_width*4-1]=255;//����Ϊ͸����
		}
		if (symbol & 0x01)
			for (i=0;i<_width*2;i++)
				_fp[(2*_height-1)*2*_width*4+4*i+3]=255;//A ����Ϊ͸����
	}
	else	
	{
		for (j=0;j<bmHeight;j++){//CBitmap�����������ֵ�õ���
			fp=memory+j*bmWidthBytes;
			a=0;b=0;
			for(i=0;i<bmWidth;i++){//˳���д���֤
				binary=fp[a];
				flag=0;
				switch(b){//�洢˳�� ��λ����λ
				case 0: if (binary & 0x80) flag=1;break;
				case 1: if (binary & 0x40) flag=1;break;
				case 2: if (binary & 0x20) flag=1;break;
				case 3: if (binary & 0x10) flag=1;break;
				case 4: if (binary & 0x08) flag=1;break;
				case 5: if (binary & 0x04) flag=1;break;
				case 6: if (binary & 0x02) flag=1;break;
				case 7: if (binary & 0x01) flag=1;break;				
				}
				_fp[j*2*_width*4+4*i]=flag*255;
				_fp[j*2*_width*4+4*i+1]=flag*255;
				_fp[j*2*_width*4+4*i+2]=flag*255;
				_fp[j*2*_width*4+4*i+3]=0;//����Ϊ��͸����
				b++;
				if (b==8){
					b=0;
					a++;
				}
			}
		    if (symbol & 0x10)
			   _fp[(j+1)*2*_width*4-1]=255;//����Ϊ͸����
		}
		if (symbol & 0x01)
			for (i=0;i<_width*2;i++)
				_fp[(2*_height-1)*2*_width*4+4*i+3]=255;//A ����Ϊ͸����
	}
	delete [] memory;
	zero=false;
	return true;
}

void PicMat::NewMat(CPoint cp,unsigned int width,unsigned int height ,COLORREF color){
	if (width<1 || height<1){
		_bias.x=_bias.y=0;
		_width=0;
		_height=0;
		_fp=NULL;
		zero=true;
	}
	else{
		_bias=cp;
		_width=width;
		_height=height;
		_fp=new unsigned char[2*_width*2*_height*4];
		int i;

		DWORD * k=(DWORD * ) _fp;
		for (i=0;i<4*_width*_height;i++)
			k[i]=color;		
		zero=false;
	}
}
void PicMat::Moveto(CPoint cp){
	if (!zero){
		_bias=cp;
	}
}
void PicMat::Move(CPoint cp){
	if (!zero){
		_bias=_bias+cp;
	}
}

void PicMat::Transparent(COLORREF color){
	int i;
	DWORD * fp=(DWORD *)_fp;

	if (!zero){
		color=color & 0x00FFFFFF;//���Ƚ�A
		for (i=0;i<_width*2*_height*2;i++)
			if ((fp[i] & 0x00FFFFFF)-color==0)
				_fp[i*4+3]=255;//����Ϊ͸��
	}
}

void PicMat::SetTransparent(unsigned char alpha,bool force){
	int i;
	if (force){
		for (i=0;i<4*_width*_height;i++){
			_fp[4*i+3]=alpha;
		}
	}
	else{
		for (i=0;i<4*_width*_height;i++){
			if(_fp[4*i+3]<255)
				_fp[4*i+3]=alpha;
		}	
	}
}
//--------------RGB-HSI-------------------------
void PicMat::RGB2HSI(unsigned char r,unsigned char g,unsigned char b,float &h,float &s,float &i){
	float theta;
	unsigned char mid;
	float PI=3.14159265359;
	//H ����Ϊ��
	theta=sqrt((r-g)*(r-g)+(r-b)*(g-b));
	if (theta){
		theta=0.5*(2*r-g-b)/theta;
		theta=acos(theta);
	}
	if (g>=b){
		h=theta;
	}
	else{
		h=2*PI-theta;
	}
	//S
	if (r<g)
		mid=r;
	else
		mid=g;
	//
	if (mid>b)
		mid=b;
	s=1-mid*3.0/(r+g+b);
	//I
	i=(r+g+b)/(3.0*255);
}
void PicMat::HSI2RGB(float h,float s,float i,unsigned char &r,unsigned char &g,unsigned char &b){
	float PI=3.14159265359;
	float fr,fg,fb;
	bool c=false;
	//int n;

	//-----------��һ��--------------
/*------------��һ��--------------
	//����I
	if(i<0)
		i=0;
	else if(i>1)
		i=1;
	//����S
	if(s<0)
		s=0;
	else if (s>1)
		s=1;
	//����H
	if(h<0){
		h=-h;
		c=true;
	}
	n=ceil((h/(2*PI)));
	h=h-n*(2*PI);
	if(c)
		h=2*PI-h;
*/
	//--------------------------------
	if ( h<PI*2/3 ){
		/* RG ����
		B=I(1-S)
		R=I(1+Scos(H)/cos(60-H))
		G=3I-(R+B) 
		*/
		fb=i*(1-s);
		fr=i*(1+s*cos(h)/cos(PI/3-h));
		fg=3*i-fb-fr;
	}
	else if(h<PI*4/3){
		/* GB ����
		R=I(1-S)
		G=I(1+Scos(H)/cos(60-H))
		B=3I-(R+G)
		*/
		h=h-2*PI/3;
		fr=i*(1-s);
		fg=i*(1+s*cos(h)/cos(PI/3-h));
		fb=3*i-fr-fg;
	}
	else{
		/* BR ����
		G=I(1-S)
		B=I(1+Scos(H)/cos(60-H))
		R=3I-(G+B)
		*/
		h=h-4*PI/3;
		fg=i*(1-s);
		fb=i*(1+s*cos(h)/cos(PI/3-h));
		fr=3*i-fg-fb;
	}
	//Debug
	if (fg>1)fg=1;
	if (fb>1) fb=1;
	if (fr>1) fr=1;
	//
	b=(unsigned char)(255*fb+0.499);
	r=(unsigned char)(255*fr+0.499);
	g=(unsigned char)(255*fg+0.499);
}
//--------------PicMat ����֧��-----------------
void PicMat::Inverse(void){
	int i;
	int j;
	for (i=0;i<4*_width*_height;i++){
		j=4*i;
		_fp[j]=255-_fp[j++];
		_fp[j]=255-_fp[j++];
		_fp[j]=255-_fp[j++];	
	}
	return;
}

void PicMat::Brightness(float rate){
	int i;
	int j;
	int te;
	for (i=0;i<4*_width*_height;i++){
		j=4*i;
		te=(int)(_fp[j]*rate+0.5);
		_fp[j++]=te>255 ? 255 : te;
		te=(int)(_fp[j]*rate+0.5);
		_fp[j++]=te>255 ? 255 : te;
		te=(int)(_fp[j]*rate+0.5);
		_fp[j++]=te>255 ? 255 : te;
	}
}
void PicMat::Saturation(float rate){
	int k;
	unsigned char * j;
	float h,s,i;
	for (k=0;k<4*_width*_height;k++){
		j=_fp+4*k;
		RGB2HSI(j[2],j[1],j[0],h,s,i);
		s=s*rate;
		if(s>1)//�������
			s=1;
		HSI2RGB(h,s,i,j[2],j[1],j[0]);
	}
}
void PicMat::Phase(float alpha){
	int k;	int n;
	bool c; float PI=3.14159265359;
	unsigned char * j;
	float h,s,i;
	for (k=0;k<4*_width*_height;k++){
		j=_fp+4*k;
		RGB2HSI(j[2],j[1],j[0],h,s,i);
		h=h+alpha;
		//h ����
		c=false;
		if(h<0){
			h=-h;
			c=true;
		}
		n=floor((h/(2*PI)));
		h=h-n*(2*PI);
		if(c)
			h=2*PI-h;
		//
		HSI2RGB(h,s,i,j[2],j[1],j[0]);
	}
}
//Ѱַ
unsigned int PicMat::Index(int x,int y,int i){
	unsigned int index;
	index=(2*_width)*(_height-y)+(x+_width);
	index=4*index+i;
	return index;
}

void PicMat::Rotate(float alpha,bool inter){
	if(zero) return;//��ֹ����
	if(!inter){
		Rotate_U(alpha);
		return;
	}//ִ�зǲ�ֵ�仯
//---------------------------------
/*
	100 50
	��50����149�ŵ�
*/
	double x1,x2;
	double y1,y2;
	unsigned int new_width;
	unsigned int new_height;
	int _width,_height;
	int x,y;
	float xx,yy;//������任����
	int nx,ny;//������ȡ��
	unsigned int index_a;//�����Ӧ������
	bool out;
	unsigned char * mem;
	float u,v;//��ֵϵ��
	int i;
	float buff[4];
	_width=this->_width;
	_height=this->_height;

	//��Եģ����
	for (i=0;i<2*_width;i++)
		_fp[i*4+3]=255;
	for (i=0;i<2*_width;i++)
		_fp[(_width*2*(2*_height-1)+i)*4+3]=255;
	for (i=0;i<2*_height;i++)
		_fp[(_width*2*i)*4+3]=255;
	for (i=0;i<2*_height;i++)
		_fp[(_width*2*(i+1)-1)*4+3]=255;
	//x'=x*cos-y*sin
	//y'=x*sin+y*cos
	x1=_width*cos(alpha)-_height*sin(alpha);
	y1=_width*sin(alpha)+_height*cos(alpha);
	x2=_width*cos(alpha)+_height*sin(alpha);
	y2=_width*sin(alpha)-_height*cos(alpha);
	//
	x1=fabs(x1);x2=fabs(x2);
	y1=fabs(y1);y2=fabs(y2);
	if (x1<x2)
		new_width=ceil(x2);
	else
		new_width=ceil(x1);
	//
	if (y1<y2)
		new_height=ceil(y2);
	else
		new_height=ceil(y1);
	//
	index_a=0;

	//���ռ�
	mem=new unsigned char [2*new_width*2*new_height*4];
	
	for (y=new_height;y>-(int)new_height;y--){
		for (x=-new_width;x<(int)new_width;x++){
			xx=x*cos(alpha)+y*sin(alpha);
			yy=-x*sin(alpha)+y*cos(alpha);
			nx=floor(xx);
			ny=floor(yy);
			//Initialize
			out=false;
			u=xx-nx;v=yy-ny;
			buff[0]=buff[1]=buff[2]=buff[3]=0;
			//
			if (nx<-_width-1){//���
				out=true;
			}
			else if(nx<-_width){//���
				if(ny<-_height){//���
					out=true;
				}
				else if(ny<-_height+1){//�ײ�
					for (i=0;i<4;i++){
						buff[i]+=_fp[Index(-_width,-_height+1,i)];//����
					}
				}
				else if(ny<_height){//�в�
					for (i=0;i<4;i++){
						buff[i]+=(1-v)*_fp[Index(-_width,ny,i)];//����
						buff[i]+=v*_fp[Index(-_width,ny+1,i)];//����
					}
				}
				else if(ny<_height+1){//�ϲ�
					for(i=0;i<4;i++){
						buff[i]+=_fp[Index(-_width,_height,i)];//����
					}
				}
				else{//���
					out=true;
				}			
			}
			else if(nx<_width-1){//�м�
				if(ny<-_height){//���
					out=true;
				}
				else if (ny<-_height+1){//�ײ�
					for (i=0;i<4;i++){
						buff[i]+=(1-u)*_fp[Index(nx,-_height+1,i)];//����
						buff[i]+=u*_fp[Index(nx+1,-_height+1,i)];//����
					}
				}
				else if (ny<_height){//�в�
					for (i=0;i<4;i++){
						buff[i]+=(1-u)*(1-v)*_fp[Index(nx,ny,i)];//����
						buff[i]+=u*(1-v)*_fp[Index(nx+1,ny,i)];//����
						buff[i]+=(1-u)*v*_fp[Index(nx,ny+1,i)];//����
						buff[i]+=u*v*_fp[Index(nx+1,ny+1,i)];//����
					}
				}
				else if (ny<_height+1){//�ϲ�
					for (i=0;i<4;i++){
						buff[i]+=(1-u)*_fp[Index(nx,_height,i)];//����
						buff[i]+=u*_fp[Index(nx+1,_height,i)];//����
					}
				}
				else{
					out=true;
				}
			}
			else if(nx<_width){//�Ҳ�
				if(ny<-_height){//���
					out=true;
				}
				else if (ny<-_height+1){//�ײ�
					for(i=0;i<4;i++){
						buff[i]+=_fp[Index(_width-1,-_height+1,i)];//����
					}
				}
				else if (ny<_height){//�в�
					for (i=0;i<4;i++){
						buff[i]+=(1-v)*_fp[Index(_width-1,ny,i)];//����
						buff[i]+=v*_fp[Index(_width-1,ny+1,i)];//����
					}
				}
				else if (ny<_height+1){//�ϲ�
					for (i=0;i<4;i++){
						buff[i]+=_fp[Index(_width-1,_height,i)];//����
					}
				}
				else{
					out=true;
				}			
			}
			else{//���
				out=true;
			}
			//--------------------
			if(out){//���
				mem[index_a]=255;
				mem[index_a+1]=255;
				mem[index_a+2]=255;
				mem[index_a+3]=255;//����͸���� ��ɫ
			}
			else{
				for (i=0;i<4;i++)//��ֵ
					mem[index_a+i]=(unsigned char)(buff[i]+0.5);			
			}
				
			index_a+=4;//��λ
		}
	}
	delete [] _fp;
	_fp=mem;
	this->_width=new_width;
	this->_height=new_height;
}
//-------------------------��ʹ��˫���Բ�ֵ��------------------------
void PicMat::Rotate_U(float alpha){
	double x1,x2;
	double y1,y2;
	unsigned int new_width;
	unsigned int new_height;
	int _width,_height;
	int x,y;
	float xx,yy;//������
	int nx,ny;//���������
	unsigned int index_a;//�����Ӧ������
	bool out;
	unsigned char * mem;
	int i;

	_width=this->_width;
	_height=this->_height;
	//x'=x*cos-y*sin
	//y'=x*sin+y*cos
	x1=_width*cos(alpha)-_height*sin(alpha);
	y1=_width*sin(alpha)+_height*cos(alpha);
	x2=_width*cos(alpha)+_height*sin(alpha);
	y2=_width*sin(alpha)-_height*cos(alpha);
	//
	x1=fabs(x1);x2=fabs(x2);
	y1=fabs(y1);y2=fabs(y2);
	if (x1<x2)
		new_width=ceil(x2);
	else
		new_width=ceil(x1);
	//
	if (y1<y2)
		new_height=ceil(y2);
	else
		new_height=ceil(y1);
	//
	index_a=0;

	//���ռ�
	mem=new unsigned char [2*new_width*2*new_height*4];
	
	for (y=new_height;y>-(int)new_height;y--){
		for (x=-new_width;x<(int)new_width;x++){
			xx=x*cos(alpha)+y*sin(alpha);
			yy=-x*sin(alpha)+y*cos(alpha);
			nx=floor(xx+0.5);
			ny=floor(yy+0.5);
			out=true;
			if(nx>=-_width && nx<_width ){
				if(ny>-_height && ny<=_height){
					for (i=0;i<4;i++)//�򵥸�ֵ
						mem[index_a+i]=_fp[Index(nx,ny,i)];
					out=false;
				}			
			}
			//��ֵ
			if (out){
				mem[index_a]=255;
				mem[index_a+1]=255;
				mem[index_a+2]=255;
				mem[index_a+3]=255;//����͸���� ��ɫ
			}
			index_a+=4;
		}
	}
	delete [] _fp;
	_fp=mem;
	this->_width=new_width;
	this->_height=new_height;
}
void PicMat::Stretch(unsigned int x,unsigned int y,bool inter){
	if(zero) return;//��ֹ����
	if (!inter){
		Stretch_U(x,y);
		return;
	}

//  100 50
//  ��50����149�ŵ�

	float kx,ky;
	unsigned char * mem;
	float xx,yy;//����任��ĸ�������
	int nx,ny;//ȡ��֮����
	int ix,iy;//�ƶ��ı���
	unsigned int index_a;//�����Ӧ��
	float u,v;int i;
	int _width,_height;
	bool out;
	float buff[4];

	_width=this->_width;
	_height=this->_height;
	//������x,y
	kx=(float)x/_width;
	ky=(float)y/_height;
	index_a=0;
	mem=new unsigned char[x*y*4*4];
	
	for (iy=y;iy>-(int)y;iy--){
		for (ix=-x;ix<(int)x;ix++){
			xx=ix/kx;
			yy=iy/ky;
			nx=floor(xx);
			ny=floor(yy);
			//Initialize
			out=false;
			u=xx-nx;v=yy-ny;
			buff[0]=buff[1]=buff[2]=buff[3]=0;
			//
			if (nx<-_width-1){//���
				out=true;
			}
			else if(nx<-_width){//���
				if(ny<-_height){//���
					out=true;
				}
				else if(ny<-_height+1){//�ײ�
					for (i=0;i<4;i++){
						buff[i]+=_fp[Index(-_width,-_height+1,i)];//����
					}
				}
				else if(ny<_height){//�в�
					for (i=0;i<4;i++){
						buff[i]+=(1-v)*_fp[Index(-_width,ny,i)];//����
						buff[i]+=v*_fp[Index(-_width,ny+1,i)];//����
					}
				}
				else if(ny<_height+1){//�ϲ�
					for(i=0;i<4;i++){
						buff[i]+=_fp[Index(-_width,_height,i)];//����
					}
				}
				else{//���
					out=true;
				}			
			}
			else if(nx<_width-1){//�м�
				if(ny<-_height){//���
					out=true;
				}
				else if (ny<-_height+1){//�ײ�
					for (i=0;i<4;i++){
						buff[i]+=(1-u)*_fp[Index(nx,-_height+1,i)];//����
						buff[i]+=u*_fp[Index(nx+1,-_height+1,i)];//����
					}
				}
				else if (ny<_height){//�в�
					for (i=0;i<4;i++){
						buff[i]+=(1-u)*(1-v)*_fp[Index(nx,ny,i)];//����
						buff[i]+=u*(1-v)*_fp[Index(nx+1,ny,i)];//����
						buff[i]+=(1-u)*v*_fp[Index(nx,ny+1,i)];//����
						buff[i]+=u*v*_fp[Index(nx+1,ny+1,i)];//����
					}
				}
				else if (ny<_height+1){//�ϲ�
					for (i=0;i<4;i++){
						buff[i]+=(1-u)*_fp[Index(nx,_height,i)];//����
						buff[i]+=u*_fp[Index(nx+1,_height,i)];//����
					}
				}
				else{
					out=true;
				}
			}
			else if(nx<_width){//�Ҳ�
				if(ny<-_height){//���
					out=true;
				}
				else if (ny<-_height+1){//�ײ�
					for(i=0;i<4;i++){
						buff[i]+=_fp[Index(_width-1,-_height+1,i)];//����
					}
				}
				else if (ny<_height){//�в�
					for (i=0;i<4;i++){
						buff[i]+=(1-v)*_fp[Index(_width-1,ny,i)];//����
						buff[i]+=v*_fp[Index(_width-1,ny+1,i)];//����
					}
				}
				else if (ny<_height+1){//�ϲ�
					for (i=0;i<4;i++){
						buff[i]+=_fp[Index(_width-1,_height,i)];//����
					}
				}
				else{
					out=true;
				}			
			}
			else{//���
				out=true;
			}
			//--------------------
			if(out){//���
				mem[index_a]=255;
				mem[index_a+1]=255;
				mem[index_a+2]=255;
				mem[index_a+3]=255;//����͸���� ��ɫ
			}
			else{
				for (i=0;i<4;i++)//��ֵ
					mem[index_a+i]=(unsigned char)(buff[i]+0.5);			
			}
			index_a+=4;//��λ
		}
	}
	delete [] _fp;
	_fp=mem;
	this->_width=x;
	this->_height=y;
}
//---------------��ʹ��˫���Ա任--------------------------
void PicMat::Stretch_U(unsigned int x,unsigned int y){
	float kx,ky;
	unsigned char * mem;
	float xx,yy;//����任��ĸ�������
	int nx,ny;//ȡ��֮����
	int ix,iy;//�ƶ��ı���
	unsigned int index_a;//�����Ӧ��
	int i;
	int _width,_height;
	bool out;

	_width=this->_width;
	_height=this->_height;
	//������x,y
	kx=(float)x/_width;
	ky=(float)y/_height;
	index_a=0;
	mem=new unsigned char[x*y*4*4];
	
	for (iy=y;iy>-(int)y;iy--){
		for (ix=-x;ix<(int)x;ix++){
			xx=ix/kx;
			yy=iy/ky;
			nx=floor(xx+0.5);
			ny=floor(yy+0.5);
			//Initialize
			out=false;
			//
			if (nx>=-_width && nx<(int)_width){
				if(ny<=_height && ny>-(int)_height){
					for (i=0;i<4;i++)
						mem[index_a+i]=_fp[Index(nx,ny,i)];
					out=true;
				}
			}
			//--------------------
			if(!out){//���
				mem[index_a]=255;
				mem[index_a+1]=255;
				mem[index_a+2]=255;
				mem[index_a+3]=255;//����͸���� ��ɫ
			}
			index_a+=4;//��λ
		}
	}
	delete [] _fp;
	_fp=mem;
	this->_width=x;
	this->_height=y;
}
void PicMat::Stretch(double ratex,double ratey,bool inter){
	unsigned int inx,iny;
	inx=floor(ratex*_width+0.5);
	iny=floor(ratey*_height+0.5);
	Stretch(inx,iny,inter);
	return;
}
void PicMat::Mirror(bool x,bool y){
	unsigned char * stack;
	unsigned char * fp;
	int index;
	int indexy;
	int i;
	int j;
	//x�᾵�淴��
	if(x){
		stack=new unsigned char [4*2*_height];
		for(j=0;j<2*_width;j++){//ˢ��
			fp=stack;
			indexy=j*4;
			//��ջ
			for (index=0;index<2*_height;index++){//ջ�����
				for(i=0;i<4;i++){
					fp[i]=_fp[indexy+i];
				}
				fp+=4;
				indexy+=4*(2*_width);
			}
			//��ջ
			indexy=j*4;
			fp-=4;
			for (index=0;index<2*_height;index++){//ջ�����
				for(i=0;i<4;i++){
					_fp[indexy+i]=fp[i];
				}
				fp-=4;
				indexy+=4*(2*_width);
			}
		}
		delete [] stack;
	}
	if (y){
		stack=new unsigned char [4*2*_width];
		for (j=0;j<2*_height;j++){//ˢ��
			fp=stack;
			indexy=(2*_width)*j*4;
			//��ջ
			for (index=0;index<2*_width;index++){//ջ�����
				for (i=0;i<4;i++){
					fp[i]=_fp[indexy+i];
				}
				fp+=4;
				indexy+=4;
			}
			indexy=(2*_width)*j*4;
			fp-=4;
			for (index=0;index<2*_width;index++){//ջ�����
				for(i=0;i<4;i++){
					_fp[indexy+i]=fp[i];
				}
				fp-=4;
				indexy+=4;
			}
		}
		delete [] stack;
	}
}

void PicMat::Cut(CPoint center,unsigned int width,unsigned int height){
	int cx,cy;
	unsigned char * mem;
	int ix,iy;//ˢ�С�ˢ��
	int nx,ny;//ȡ��֮����
	unsigned int index_a;//�����Ӧ��
	int i;
	int _width,_height;
	bool out;

	_width=this->_width;
	_height=this->_height;
	//������x,y
	cx=center.x;
	cy=center.y;
	index_a=0;
	mem=new unsigned char[width*height*4*4];
	
	for (iy=height;iy>-(int)height;iy--){
		for (ix=-width;ix<(int)width;ix++){
			nx=ix+cx;
			ny=iy+cy;
			//Initialize
			out=false;
			//
			if (nx>=-_width && nx<(int)_width){
				if(ny<=_height && ny>-(int)_height){
					for (i=0;i<4;i++)
						mem[index_a+i]=_fp[Index(nx,ny,i)];
					out=true;
				}
			}
			//--------------------
			if(!out){//���
				mem[index_a]=255;
				mem[index_a+1]=255;
				mem[index_a+2]=255;
				mem[index_a+3]=255;//����͸���� ��ɫ
			}
			index_a+=4;//��λ
		}
	}
	delete [] _fp;
	_fp=mem;
	this->_width=width;
	this->_height=height;
}
void PicMat::Affine(CPoint right_top,CPoint right_bottom,bool inter){
	if(zero) return;//��ֹ����


//  100 50
//  ��50����149�ŵ�

	float kx,ky;//x ��任����
	float jx,jy;//y ��任����
	unsigned char * mem;
	unsigned int new_width,new_height;
	float xx,yy;//����任��ĸ�������
	int nx,ny;//ȡ��֮����
	int ix,iy;//�ƶ��ı���
	unsigned int index_a;//�����Ӧ��
	float u,v;int i;
	int _width,_height;
	bool out;
	float buff[4];float Det;
	int x1,x2,y1,y2;
	_width=this->_width;
	_height=this->_height;
	//��û�����С
	x1=abs(right_top.x);x2=abs(right_bottom.x);
	y1=abs(right_top.y);y2=abs(right_bottom.y);
	if (x1<x2)
		new_width=x2;
	else
		new_width=x1;
	//
	if (y1<y2)
		new_height=y2;
	else
		new_height=y1;
	//
	//���x,y
	jx=(float)(right_top.x-right_bottom.x)/(2*_height);
	jy=(float)(right_top.y-right_bottom.y)/(2*_height);
	kx=(float)(right_top.x+right_bottom.x)/(2*_width);
	ky=(float)(right_top.y+right_bottom.y)/(2*_width);
	Det=jx*ky-jy*kx;
	mem=new unsigned char[new_width*new_height*4*4];

	//------------------��ĸΪ��Ĵ�����-------------
	if(Det==0){//��ĸΪ��Ĵ�����
		for(i=0;i<4*new_width*new_height;i++){
			mem[4*i]=255;//��ɫ͸��
			mem[4*i+1]=255;
			mem[4*i+2]=255;
			mem[4*i+3]=255;
		}
		delete [] _fp;
		_fp=mem;
		this->_width=new_width;
		this->_height=new_height;
		return;
	}
	//---------------------end------------------------
	//�����Բ�ֵ ���Ƽ��ķ���
	if (!inter){
		Affine_U(kx,ky,jx,jy,new_width,new_height,mem);
		return;
	}
	//Next
	index_a=0;
	//��Եģ����
	for (i=0;i<2*_width;i++)
		_fp[i*4+3]=255;
	for (i=0;i<2*_width;i++)
		_fp[(_width*2*(2*_height-1)+i)*4+3]=255;
	for (i=0;i<2*_height;i++)
		_fp[(_width*2*i)*4+3]=255;
	for (i=0;i<2*_height;i++)
		_fp[(_width*2*(i+1)-1)*4+3]=255;
	
	for (iy=new_height;iy>-(int)new_height;iy--){
		for (ix=-new_width;ix<(int)new_width;ix++){
			//���任
			xx=(-jy*ix+jx*iy)/Det;
			yy=(+ky*ix-kx*iy)/Det;
			//������
			nx=floor(xx);
			ny=floor(yy);
			//Initialize
			out=false;
			u=xx-nx;v=yy-ny;
			buff[0]=buff[1]=buff[2]=buff[3]=0;
			//
			if (nx<-_width-1){//���
				out=true;
			}
			else if(nx<-_width){//���
				if(ny<-_height){//���
					out=true;
				}
				else if(ny<-_height+1){//�ײ�
					for (i=0;i<4;i++){
						buff[i]+=_fp[Index(-_width,-_height+1,i)];//����
					}
				}
				else if(ny<_height){//�в�
					for (i=0;i<4;i++){
						buff[i]+=(1-v)*_fp[Index(-_width,ny,i)];//����
						buff[i]+=v*_fp[Index(-_width,ny+1,i)];//����
					}
				}
				else if(ny<_height+1){//�ϲ�
					for(i=0;i<4;i++){
						buff[i]+=_fp[Index(-_width,_height,i)];//����
					}
				}
				else{//���
					out=true;
				}			
			}
			else if(nx<_width-1){//�м�
				if(ny<-_height){//���
					out=true;
				}
				else if (ny<-_height+1){//�ײ�
					for (i=0;i<4;i++){
						buff[i]+=(1-u)*_fp[Index(nx,-_height+1,i)];//����
						buff[i]+=u*_fp[Index(nx+1,-_height+1,i)];//����
					}
				}
				else if (ny<_height){//�в�
					for (i=0;i<4;i++){
						buff[i]+=(1-u)*(1-v)*_fp[Index(nx,ny,i)];//����
						buff[i]+=u*(1-v)*_fp[Index(nx+1,ny,i)];//����
						buff[i]+=(1-u)*v*_fp[Index(nx,ny+1,i)];//����
						buff[i]+=u*v*_fp[Index(nx+1,ny+1,i)];//����
					}
				}
				else if (ny<_height+1){//�ϲ�
					for (i=0;i<4;i++){
						buff[i]+=(1-u)*_fp[Index(nx,_height,i)];//����
						buff[i]+=u*_fp[Index(nx+1,_height,i)];//����
					}
				}
				else{
					out=true;
				}
			}
			else if(nx<_width){//�Ҳ�
				if(ny<-_height){//���
					out=true;
				}
				else if (ny<-_height+1){//�ײ�
					for(i=0;i<4;i++){
						buff[i]+=_fp[Index(_width-1,-_height+1,i)];//����
					}
				}
				else if (ny<_height){//�в�
					for (i=0;i<4;i++){
						buff[i]+=(1-v)*_fp[Index(_width-1,ny,i)];//����
						buff[i]+=v*_fp[Index(_width-1,ny+1,i)];//����
					}
				}
				else if (ny<_height+1){//�ϲ�
					for (i=0;i<4;i++){
						buff[i]+=_fp[Index(_width-1,_height,i)];//����
					}
				}
				else{
					out=true;
				}			
			}
			else{//���
				out=true;
			}
			//--------------------
			if(out){//���
				mem[index_a]=255;
				mem[index_a+1]=255;
				mem[index_a+2]=255;
				mem[index_a+3]=255;//����͸���� ��ɫ
			}
			else{
				for (i=0;i<4;i++)//��ֵ
					mem[index_a+i]=(unsigned char)(buff[i]+0.5);			
			}
			index_a+=4;//��λ
		}
	}
	delete [] _fp;
	_fp=mem;
	this->_width=new_width;
	this->_height=new_height;
}
void PicMat::Affine_U(float kx,float ky,float jx,float jy,int W,int H,unsigned char * fp){
	
	float xx,yy;//����任��ĸ�������
	int nx,ny;//ȡ��֮����
	int ix,iy;//�ƶ��ı���
	unsigned int index_a;//�����Ӧ��
	int i;
	int _width,_height;
	bool out;
	float Det;

	
	_width=this->_width;
	_height=this->_height;
	Det=jx*ky-jy*kx;

	index_a=0;
	for (iy=H;iy>-H;iy--){
		for (ix=-W;ix<W;ix++){
			//���任
			xx=(-jy*ix+jx*iy)/Det;
			yy=(+ky*ix-kx*iy)/Det;
			//������
			nx=floor(xx+0.5);
			ny=floor(yy+0.5);
			//Initialize
			out=false;
			//
			if (nx>=-_width && nx<_width){
				if(ny<=_height && ny>-_height){
					out=true;
					fp[index_a]=_fp[Index(nx,ny,0)];
					fp[index_a+1]=_fp[Index(nx,ny,1)];
					fp[index_a+2]=_fp[Index(nx,ny,2)];
					fp[index_a+3]=_fp[Index(nx,ny,3)];
				}
			}
			//--------------------
			if(!out){//���
				fp[index_a]=255;
				fp[index_a+1]=255;
				fp[index_a+2]=255;
				fp[index_a+3]=255;//����͸���� ��ɫ
			}
			index_a+=4;//��λ
		}
	}
	delete [] _fp;
	_fp=fp;
	this->_width=(unsigned int)W;
	this->_height=(unsigned int)H;
}
//��һԪ���η���
void PicMat::Quad(float a,float b,float c,float * out,int &n){
	double delta;
	if(a==0){
		if(b==0){
			if(c==0)
				n=-1;//�����
			else
				n=0;//�޽�
		}
		else
		{
			out[0]=-c/b;
			n=1;
		}
	}
	else{
		delta=b*b-4*a*c;
		if(delta<0){
			n=0;//�޽�
		}
		else{
			delta=sqrt(delta);
			out[0]=(-b+delta)/(2*a);
			out[1]=(-b-delta)/(2*a);
			n=2;//�н�
		}
	}
}
void PicMat::Perspective(CPoint left_top,CPoint left_bottom,CPoint right_top,CPoint right_bottom,bool inter){
	if(zero) return;
	if (inter)
		Perspective_A(left_top,left_bottom,right_top,right_bottom);
	else
		Perspective_U(left_top,left_bottom,right_top,right_bottom);
	return;
}
void PicMat::Perspective_A(CPoint left_top,CPoint left_bottom,CPoint right_top,CPoint right_bottom){

//  100 50
//  ��50����149�ŵ�
	
	unsigned char * mem;
	unsigned int new_width,new_height;
	float xx,yy;//����任��ĸ�������
	int nx,ny;//ȡ��֮����
	int ix,iy;//�ƶ��ı���
	unsigned int index_a;//�����Ӧ��
	float u,v;int i;
	int _width,_height;
	bool out;
	float buff;
	float ans[2],ans2[2];
	int n;//��Ҫ��
	int x1,x2,y1,y2;
	int vector_x,vector_y;
	float a,b,c;
	
	//ʸ������
	float alpha_x,alpha_y;
	float belta1_x,belta1_y;
	float belta2_x,belta2_y;
	float _alpha_x,_alpha_y;

	_width=this->_width;
	_height=this->_height;
	
	//��û�����С
	x1=x2=left_top.x;
	y1=y2=left_top.y;
	//1
	if(left_bottom.x<x1) x1=left_bottom.x;
	else if(left_bottom.x>x2) x2=left_bottom.x;
	if(left_bottom.y<y1) y1=left_bottom.y;
	else if(left_bottom.y>y2) y2=left_bottom.y;
	//2
	if(right_top.x<x1) x1=right_top.x;
	else if(right_top.x>x2) x2=right_top.x;
	if(right_top.y<y1) y1=right_top.y;
	else if(right_top.y>y2) y2=right_top.y;
	//3
	if(right_bottom.x<x1) x1=right_bottom.x;
	else if(right_bottom.x>x2) x2=right_bottom.x;
	if(right_bottom.y<y1) y1=right_bottom.y;
	else if(right_bottom.y>y2) y2=right_bottom.y;
	
	//������ͱ任���λ��
	new_width=(x2-x1)/2+(x2-x1)%2;
	new_height=(y2-y1)/2+(y2-y1)%2;
	vector_x=(x2+x1)/2+(x2+x1)%2;
	vector_y=(y2+y1)/2+(y2+y1)%2;
	
	alpha_x=left_top.x-left_bottom.x;alpha_y=left_top.y-left_bottom.y;
	belta1_x=right_top.x-left_top.x;belta1_y=right_top.y-left_top.y;
	belta2_x=right_bottom.x-left_bottom.x;belta2_y=right_bottom.y-left_bottom.y;
	
	a=(belta1_x-belta2_x)*alpha_y-(belta1_y-belta2_y)*alpha_x;
	//Next
	index_a=0;
	mem=new unsigned char[new_width*new_height*4*4];
	//��Եģ����
	for (i=0;i<2*_width;i++)
		_fp[i*4+3]=255;
	for (i=0;i<2*_width;i++)
		_fp[(_width*2*(2*_height-1)+i)*4+3]=255;
	for (i=0;i<2*_height;i++)
		_fp[(_width*2*i)*4+3]=255;
	for (i=0;i<2*_height;i++)
		_fp[(_width*2*(i+1)-1)*4+3]=255;
	
	for (iy=new_height;iy>-(int)new_height;iy--){
		for (ix=-new_width;ix<(int)new_width;ix++){
			//
			_alpha_x=ix+(vector_x-left_bottom.x);
			_alpha_y=iy+(vector_y-left_bottom.y);
			b=_alpha_x*(belta1_y-belta2_y)-_alpha_y*(belta1_x-belta2_x)+(alpha_y*belta2_x-alpha_x*belta2_y);
			c=belta2_y*_alpha_x-belta2_x*_alpha_y;
			Quad(a,b,c,ans,n);
			for(i=0;i<n;i++){
				if(ans[i]>0 && ans[i]<1){//�����ײ�������ȡ��
					ans2[i]=(_alpha_x-ans[i]*alpha_x)/(ans[i]*belta1_x+(1-ans[i])*belta2_x);
					if(ans2[i]>=0 && ans2[i]<1)//�����ȡ�� �Ҳ಻��ȡ��
						break;
				}
			}
			out=false;
			if(i<n){//�ɹ�����
				xx=ans2[i]*2*_width;
				yy=ans[i]*2*_height;
				nx=floor(xx);
				ny=floor(yy);
				u=xx-nx;v=yy-ny;
				nx=nx-_width;ny=ny-_height;
			}
			else
				out=true;
			buff=0;
			//
			if (nx==_width){//���
				out=true;
			}
			else if (ny==-_height){
				out=true;
			}
			//�����ж����
			if(out){
				mem[index_a]=255;
				mem[index_a+1]=255;
				mem[index_a+2]=255;
				mem[index_a+3]=255;//����͸���� ��ɫ
			}
			else{
				for (i=0;i<4;i++){
					buff=0;
					buff+=(1-u)*(1-v)*_fp[Index(nx,ny,i)];//����
					buff+=u*(1-v)*_fp[Index(nx+1,ny,i)];//����
					buff+=(1-u)*v*_fp[Index(nx,ny+1,i)];//����
					buff+=u*v*_fp[Index(nx+1,ny+1,i)];//����
					mem[index_a+i]=(unsigned char)(buff+0.5);
				}
			}
			index_a+=4;//��λ
		}
	}
	delete [] _fp;
	_fp=mem;
	this->_width=new_width;
	this->_height=new_height;
	this->Move(CPoint(vector_x,-vector_y));
}
void PicMat::Perspective_U(CPoint left_top,CPoint left_bottom,CPoint right_top,CPoint right_bottom){

//  100 50
//  ��50����149�ŵ�
	
	unsigned char * mem;
	unsigned int new_width,new_height;
	float xx,yy;//����任��ĸ�������
	int nx,ny;//ȡ��֮����
	int ix,iy;//�ƶ��ı���
	unsigned int index_a;//�����Ӧ��
	int i;
	int _width,_height;
	bool out;
	float ans[2],ans2[2];
	int n;//��Ҫ��
	int x1,x2,y1,y2;
	int vector_x,vector_y;
	float a,b,c;
	
	//ʸ������
	float alpha_x,alpha_y;
	float belta1_x,belta1_y;
	float belta2_x,belta2_y;
	float _alpha_x,_alpha_y;

	_width=this->_width;
	_height=this->_height;
	
	//��û�����С
	x1=x2=left_top.x;
	y1=y2=left_top.y;
	//1
	if(left_bottom.x<x1) x1=left_bottom.x;
	else if(left_bottom.x>x2) x2=left_bottom.x;
	if(left_bottom.y<y1) y1=left_bottom.y;
	else if(left_bottom.y>y2) y2=left_bottom.y;
	//2
	if(right_top.x<x1) x1=right_top.x;
	else if(right_top.x>x2) x2=right_top.x;
	if(right_top.y<y1) y1=right_top.y;
	else if(right_top.y>y2) y2=right_top.y;
	//3
	if(right_bottom.x<x1) x1=right_bottom.x;
	else if(right_bottom.x>x2) x2=right_bottom.x;
	if(right_bottom.y<y1) y1=right_bottom.y;
	else if(right_bottom.y>y2) y2=right_bottom.y;
	
	//������ͱ任���λ��
	new_width=(x2-x1)/2+(x2-x1)%2;
	new_height=(y2-y1)/2+(y2-y1)%2;
	vector_x=(x2+x1)/2+(x2+x1)%2;
	vector_y=(y2+y1)/2+(y2+y1)%2;
	
	alpha_x=left_top.x-left_bottom.x;alpha_y=left_top.y-left_bottom.y;
	belta1_x=right_top.x-left_top.x;belta1_y=right_top.y-left_top.y;
	belta2_x=right_bottom.x-left_bottom.x;belta2_y=right_bottom.y-left_bottom.y;
	
	a=(belta1_x-belta2_x)*alpha_y-(belta1_y-belta2_y)*alpha_x;
	//Next
	index_a=0;
	mem=new unsigned char[new_width*new_height*4*4];
	//�����б�Եģ����
	for (iy=new_height;iy>-(int)new_height;iy--){
		for (ix=-new_width;ix<(int)new_width;ix++){
			//
			_alpha_x=ix+(vector_x-left_bottom.x);
			_alpha_y=iy+(vector_y-left_bottom.y);
			b=_alpha_x*(belta1_y-belta2_y)-_alpha_y*(belta1_x-belta2_x)+(alpha_y*belta2_x-alpha_x*belta2_y);
			c=belta2_y*_alpha_x-belta2_x*_alpha_y;
			Quad(a,b,c,ans,n);
			for(i=0;i<n;i++){
				if(ans[i]>0 && ans[i]<=1){//������ȡ�����ײ�����ȡ��
					ans2[i]=(_alpha_x-ans[i]*alpha_x)/(ans[i]*belta1_x+(1-ans[i])*belta2_x);
					if(ans2[i]>=0 && ans2[i]<1)//�����ȡ�����Ҳ಻��ȡ��
						break;
				}
			}
			out=false;
			if(i<n){//�ɹ�����
				xx=ans2[i]*2*_width;
				yy=ans[i]*2*_height;
				nx=floor(xx+0.5);
				ny=floor(yy+0.5);
				nx=nx-_width;ny=ny-_height;
			}
			else
				out=true;
			//
			if (nx==_width){//���
				out=true;
			}
			else if (ny==-_height){
				out=true;
			}
			//�����ж����
			if(out){
				mem[index_a]=255;
				mem[index_a+1]=255;
				mem[index_a+2]=255;
				mem[index_a+3]=255;//����͸���� ��ɫ
			}
			else{
				for (i=0;i<4;i++){
					mem[index_a+i]=_fp[Index(nx,ny,i)];
				}
			}
			index_a+=4;//��λ
		}
	}
	delete [] _fp;
	_fp=mem;
	this->_width=new_width;
	this->_height=new_height;
	this->Move(CPoint(vector_x,-vector_y));
}

