

#include "stdafx.h"
#include "SC_datatype_enum.h"


void TSC_enumval::streamout(QBinTagWriter &writer)
{
	writer.write_int32(val);
}

void TSC_enumval::streamin(QBinTagReader &reader)
{
	val=reader.read_int32();
}


TSC_datatype_genericenum* TSC_datatype_genericenum::duplicate()
{
	return new TSC_datatype_genericenum(G_name());
}
