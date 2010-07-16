#include "WebHandler.h"
#include "Html.h"
#include "WEBServer.h"
#include "CTime.h"
#include "URLEncoding.h"

CWebHandler::CWebHandler():
_stop(false)
{
	this->setName("WEB Handler");
}

CWebHandler::~CWebHandler()
{
}

void CWebHandler::Close()
{
	JTCSynchronized sync(*this);
	_stop = true;
	this->notify();
}

void CWebHandler::run()
{
	JTCSynchronized sync(*this);
	
	char buffer[MAX_HTTP_REQUEST_SIZE];
	CHttpReply reply;
	CDataBuffer reply_buf;

	while (!_stop)
	{
		this->wait();

		InitReply(reply);
		
		if(_job_queue.size() == 0){
			continue;
		}

		TCPSocket* sock = _job_queue.front();

		START_TRY
			HandleRequest(sock, buffer,reply);
		END_TRY_START_CATCH(e)
			//send error page to user
			CContentGenerator::Generate_Error_Line(reply_buf,e.what());
		END_CATCH

		reply.Finalize(reply_buf);
		sock->Send(reply_buf.GetBuffer(),reply_buf.GetLength());
		reply.Reset();
		memset(buffer,0,MAX_HTTP_REQUEST_SIZE);
		RemoveFromJobQueue();
		if (sock != NULL){
			delete sock;
		}
	}
}

void CWebHandler::InitReply(CHttpReply& reply)
{
	reply.RemoveAllHeaders();
	reply.SetContentType(CT_TEXT_HTML);
	reply.SetStatusCode(STATUS_OK);
	reply.SetVersion(HTTP_1_0);
	reply.AddHeader("Connection","Close");
}

void CWebHandler::Signal()
{
	JTCSynchronized sync(*this);
	this->notify();
}

void CWebHandler::HandleRequest(TCPSocket* sock, char* buffer,CHttpReply& reply)
{
	int len = 0;
	START_TRY
		len = sock->Recv(buffer,MAX_HTTP_REQUEST_SIZE,INFINITE);
	END_TRY_START_CATCH_SOCKET(e)
		CWEBServer::GetInstance().GetLog().Log(LOG_LEVEL_ERROR,"Socket exception in WEBHandler. Code: %d",e.GetErrorCode());	
	END_CATCH

	if (len > MAX_HTTP_REQUEST_SIZE || len == 0){
		//log error and return 404 to client
		return;
	}
	
	CHttpRequest request;
	CHttpParser parser(request,buffer,len);
	if (!parser.IsLegalRequest()){
		//return error to client
		throw CEIBException(GeneralError,"Error in request parsing");
	}

	CHttpHeader auth;
	if(!request.GetHeader(AUTHORIZATION_HEADER,auth)){
		reply.AddHeader(AUTHENTICATION_HEADER,"Basic");
		reply.SetStatusCode(STATUS_NOT_AUTHORIZED);
		return;
	}
	
	CDigest digest(ALGORITHM_BASE64);
	int index = auth.GetValue().Find("Basic ");
	CString cipher = auth.GetValue().SubString(index + 6,auth.GetValue().GetLength() - index - 6);
	CString clear  = digest.Decode(cipher);
	//user name and password
	CHttpHeader login(clear);

	//find is user exist and password valid
	CUser user;
	if (!CWEBServer::GetInstance().GetUsersDB().AuthenticateUser(login.GetName(),login.GetValue(),user)){
		reply.AddHeader(AUTHENTICATION_HEADER,"Basic");
		reply.SetStatusCode(STATUS_NOT_AUTHORIZED);
		return;
	}

	reply.SetContentType(CT_TEXT_HTML);
	reply.SetStatusCode(STATUS_OK);
	reply.SetVersion(HTTP_1_0);

	if(request.GetRequestURI() == "/favicon.ico" && request.GetNumParams() == 0){
		HandleFavoritsIconRequest(reply);
		return;
	}
	
	//logic of parameters
	CreateContent(request,reply);
}

void CWebHandler::HandleFavoritsIconRequest(CHttpReply& reply)
{
	reply.SetVersion(HTTP_1_0);
	reply.SetStatusCode(STATUS_OK);
	reply.SetContentType(CT_IMAGE_X_ICON);

	reply.AddHeader("Accept-Ranges","bytes");

	CString icon_file = CWEBServer::GetInstance().GetConfig().GetImagesFolder();
	icon_file += "\\favicon.ico";

	ifstream file (icon_file.GetBuffer(), ios::out | ios::app | ios::binary);
	if(file.is_open()){
		file.seekg(0, std::ios_base::end);
		int size = file.tellg();
		char* memblock = new char [size];
		file.seekg(0, ios::beg);
		file.read (memblock, size);
		file.close();
		reply.GetContent().Clear();
		reply.GetContent().Add(memblock,size);
	}
}

void CWebHandler::CreateContent(CHttpRequest& request,CHttpReply& reply)
{
	CHttpParameter p;
	CString err;

	if(request.GetNumParams() == 0){
		CContentGenerator gen;
		gen.Page_Main(reply.GetContent(),GetCurrentDomain(),err);
		return;
	}

	if(!request.GetParameter(PARAM_NAME_COMMAND,p)){
		//redirect to main page
		reply.Redirect("/");
		return;
	}

	CStatsDB db;
	CEIBObjectRecord rec;
	CContentGenerator gen;
	CHttpParameter p1,p2,p3;
	unsigned char apci[MAX_EIB_VALUE_LEN];	
	unsigned char apci_len;
	CString norm_param;
	CEibAddress addr;
	CTime datetime;

	switch (p.GetValue().ToInt())
	{
	case PARAM_COMMAND_VALUE_GLOBAL_HISTORY:
		GetHisotryFromEIB(db,err);
		gen.Page_HistoryGlobal(reply.GetContent(),db,err);
		break;
	case PARAM_COMMAND_VALUE_FUNCTION_HISTORY:
		if(!CWEBServer::GetInstance().IsConnected()){
			err += "Eib server is not connected";
		}
		
		if(!request.GetParameter(PARAM_NAME_FUNCTION,p1)){
			gen.Form_HistoryPerFunction(reply.GetContent(),err);
			return;
		}
		norm_param = URLEncoder::Decode(p1.GetValue());
		addr = CEibAddress(norm_param);
		GetHisotryFromEIB(db,err);
		if(!db.GetRecord(addr,rec)){
			err += "No Entries found for EIB Address: ";
			err += norm_param;
		}
		gen.Page_HistoryPerFunction(reply.GetContent(),addr,rec,err);
		break;
	case PARAM_COMMAND_VALUE_SEND_COMMAND:
		if(!CWEBServer::GetInstance().IsConnected()){
			err += "Eib server is not connected";
		}
		//	get first http parameter, PARAM_NAME_FUNCTION_DEST_ADDRESS
		//	if parameter is missing, load page with error message
		if(!request.GetParameter(PARAM_NAME_FUNCTION_DEST_ADDRESS,p1)) {		
			gen.Form_SendCommandToEIB(reply.GetContent(),err);
			return;
		}
		//	get second http parameter, PARAM_NAME_FUNCTION_DEST_ADDRESS
		//	if parameter is missing, load page with error message
		if(!request.GetParameter(PARAM_NAME_FUNCTION_APCI,p2)){
			err += "Parameter \"";
			err += PARAM_NAME_FUNCTION_APCI;
			err += "\" is missing from request";
			gen.Form_SendCommandToEIB(reply.GetContent(),err);
			return;
		}	
		//	get apci Hex value from http parameter p2
		//	if apci parameter value cannot be converted to Hex
		//	return to Send Command form with error message		
		if(!GetByteArrayFromHexString(p2.GetValue(),apci,apci_len)){			
			gen.Form_SendCommandToEIB(reply.GetContent(),err);
			return;
		}
		//	we have valid address and apci value.
		//	send command. Generate Ack page - not successful
		if(!SendEIBCommand(p1.GetValue(),apci,apci_len,err)){
			gen.Page_AckCommandToEIB(reply.GetContent(), false, err);
			return;
		}
		//	Generate Ack page - successful
		gen.Page_AckCommandToEIB(reply.GetContent(), true, err);
		break;
	case PARAM_COMMAND_VALUE_SCHEDULE_COMMAND:		
		if(!CWEBServer::GetInstance().IsConnected()){
			err += "Eib server is not connected";
		}
	
		if(!request.GetParameter(PARAM_NAME_FUNCTION_DEST_ADDRESS,p1)) {
			gen.Form_ScheduleCommand(reply.GetContent(),err);
			return;
		}
		if(!request.GetParameter(PARAM_NAME_FUNCTION_APCI,p2)){
			err += "Parameter \"";
			err += PARAM_NAME_FUNCTION_APCI;
			err += " missing from request";
			gen.Form_ScheduleCommand(reply.GetContent(),err);
			return;
		}
		if(!request.GetParameter(PARAM_NAME_FUNCTION_DATETIME,p3)){
			err += "Parameter \"";
			err += PARAM_NAME_FUNCTION_DATETIME;
			err += " missing from request";
			gen.Form_ScheduleCommand(reply.GetContent(),err);
			return;
		}
		
		if(!GetByteArrayFromHexString(p2.GetValue(),apci, apci_len)){
			err += "Illegal unsigned short representation";
			gen.Form_ScheduleCommand(reply.GetContent(),err);
			return;
		}
		//YBYB TODO: check datetime parameter sanity
		norm_param = URLEncoder::Decode(p3.GetValue());
		datetime = CTime(norm_param.GetBuffer(),true);
		//YBYB TODO: Schedule the command
		//YBYB change AddScheduledCommand to get int val_len?
		if(!CWEBServer::GetInstance().GetCollector()->AddScheduledCommand(datetime,
																	 CEibAddress(URLEncoder::Decode(p1.GetValue())),
																	 apci,
																	 apci_len,
																	 err)){
		    gen.Form_ScheduleCommand(reply.GetContent(),err);
			return;
		}
		reply.Redirect(CString("/?") + CString(PARAM_NAME_COMMAND) + CString("=") + CString(PARAM_COMMAND_VALUE_SCHEDULE_COMMAND));
		break;
	default:
		reply.Redirect("/");
		break;
	}
}

bool CWebHandler::GetByteArrayFromHexString(const CString& str, unsigned char *val, unsigned char &val_len)
{
	if(str.FindFirstOf("0x") != 0){
		return false;	
	}
	val_len = 0;
	
	CString temp_str(str.SubString(2,str.GetLength() - 2));
	//	trim leading zeroes
	temp_str.TrimStart('0');
	//	round up to nearest even number
	if ( temp_str.GetLength() % 2 != 0){
		temp_str = CString("0") + temp_str;
	}
	ASSERT_ERROR(temp_str.GetLength() % 2 == 0,"Must be even!!!!");

	//	take each 2-Hex-digit pair from string and convert to one byte
	for (int i = 0; i < temp_str.GetLength(); i += 2)
	{
		if(!IS_HEX_DIGIT(temp_str[i]) || !IS_HEX_DIGIT(temp_str[i + 1]))
		{
			return false;
		}
		val[i / 2] = HexToChar(temp_str.SubString(i,2));
		++val_len;
	}	
	return true;
}

unsigned char CWebHandler::HexToChar(const CString& hexNumber)
{	
	unsigned char high_digit = hexNumber[0];
	unsigned char low_digit	= hexNumber[1];
	int lowOrderValue = GetDigitValue(low_digit);
	int highOrderValue = GetDigitValue(high_digit);
	
	unsigned char res = (unsigned char)(lowOrderValue + 16 * highOrderValue);
	return res;	
}


int CWebHandler::GetDigitValue (char digit)
{
	if (digit >= '0' && digit <= '9'){
      return (digit - '0');
	}
	else if (digit >= 'A' && digit <= 'F'){		
		return (digit - 'A' + 10);
	}
	else if (digit >= 'a' && digit <= 'f'){
		return (digit - 'a' + 10);
	}

	return -1;
}

bool CWebHandler::SendEIBCommand(const CString& addr, unsigned char *apci, unsigned char apci_len, CString& err)
{
	START_TRY
		CEibAddress dest_addr(URLEncoder::Decode(addr));
	
		if(CWEBServer::GetInstance().SendEIBNetwork(dest_addr,apci,apci_len,NON_BLOCKING) == 0){
			err += "Eib server is not connected";
			return false;
		}
	END_TRY_START_CATCH(e)
		err += e.what();
		return false;
	END_CATCH

	return true;
}

void CWebHandler::GetHisotryFromEIB(CStatsDB& db, CString& err)
{
	try
	{
		if(!CWEBServer::GetInstance().IsConnected()){
			err += "EIB Server is not Connected";
			return;
		}
		db = CWEBServer::GetInstance().GetCollector()->GetStatsDB();
	}
	catch (CEIBException& e)
	{
		err += e.what();
	}
}

void CWebHandler::RemoveFromJobQueue()
{
	_job_queue.pop();
}

void CWebHandler::AddToJobQueue(TCPSocket* job)
{
	JTCSynchronized sync(*this);
	
	ASSERT_ERROR(job != NULL,"NULL Socket cannot be added to queue")
	_job_queue.push(job);
}

const CString& CWebHandler::GetCurrentDomain() const
{
	return CWEBServer::GetInstance().GetDomain();
}
