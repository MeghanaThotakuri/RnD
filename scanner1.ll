
%filenames="scanner"
%lex-source="scanner.cc"


%%

[0-9]+	{
		ParserBase::STYPE__ * val = getSval();
		val->integer_value = atoi(matched().c_str());
		// cout<<"NUM\t"<<(val->integer_value)<<endl;
		return Parser::NUM; 
	}


[[:alpha:]_\$][[:alpha:][:digit:]_.\$]* {
		ParserBase::STYPE__ * val = getSval();
		val->string_value = new std::string(matched());
		// cout<<"ID\t"<<*(val->string_value)<<endl;
		return Parser::ID;
	}


[=,{}():;]	{
		return matched()[0];
	}

%% {
	return Parser::PPERCENT;
}


[ \t\n\f]	{ }


.	{ 
		string error_message;
		error_message =  "Illegal character `" + matched();
		error_message += "' on line " + lineNr();
	    cerr << error_message<<endl;	
	    exit(1);
	}
