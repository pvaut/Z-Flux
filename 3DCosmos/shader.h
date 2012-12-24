

class TShader
{
private:
	GLuint shadid_vertex,shadid_fragment,progid;
private:
	void setsource(GLuint id, StrPtr ifilename);
	void printLog(GLuint obj);
public:
	TShader();
	~TShader();
	void loadsource_vertex(StrPtr ifilename);
	void loadsource_fragment(StrPtr ifilename);
	void compile();
	void use();
};