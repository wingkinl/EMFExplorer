#ifndef EMF_REC_ACCESS_H
#define EMF_REC_ACCESS_H

#include <memory>
#include <vector>

class EMFRecAccess
{
public:
	EMFRecAccess() = default;
	virtual ~EMFRecAccess() = default;
public:

};

class EMFAccess final
{
public:
	EMFAccess();
	~EMFAccess();
public:
protected:
	using EmfRecArray	= std::vector<std::shared_ptr<EMFRecAccess>>;

	std::shared_ptr<Gdiplus::Metafile>	m_metafile;
	EmfRecArray		m_records;
};

#endif // EMF_REC_ACCESS_H
