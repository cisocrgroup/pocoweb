#ifndef pcw_Json_hpp__
#define pcw_Json_hpp__

namespace pcw {
	class Json: public boost::property_tree::ptree {
	public:
		Json() {this->put("api-version", PCW_API_VERSION);}
		void write(std::ostream& os) const {
			boost::property_tree::write_json(os, *this);
		}
		std::string string() const {
			std::stringstream os;
			write(os);
			return os.str();
		}
	};
}

#endif // pcw_Json_hpp__
