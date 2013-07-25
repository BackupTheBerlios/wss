#ifndef IMAGE_H
#define IMAGE_H

class image_c: public object_c
{
	public:
	image_c ();
	virtual void draw ( lamb_c&);
	void load ( string&);
	size_t columns;
	size_t rows;
	string file_name;
	void convert ( string );
	void convert_ppm_to_image ( string&, string&);
	void convert_pdf_to_png ( string );
	void convert_png_to_ima ( string );
	int page;
	spline_c frame;
	static const unsigned long type;
	virtual unsigned long get_type ();
};

#endif 