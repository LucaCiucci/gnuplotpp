/* LC_NOTICE_BEGIN
LC_NOTICE_END */

#include <iostream>
#include <stdexcept>

int safe_main_3(int argc, char** argv);

int main(int argc, char** argv)
{
	try
	{
		return safe_main_3(argc, argv);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Uncaught Exception thrown from safe_main(): " << e.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Uncaught Unknown Exception thrown from safe_main()" << std::endl;
	}
	return EXIT_FAILURE;
}

#include <streambuf>
#include <vector>
#include <sstream>

#include <cstring>

// https://blog.csdn.net/tangyin025/article/details/50487544
class MyStream : public std::streambuf
{
public:

	void aaa()
	{
	}

	MyStream()
	{
		m_buff.resize(20);
		this->setp(m_buff.data(), m_buff.data() + m_buff.capacity());
		//m_buff.clear();
	}

	std::vector<char> m_buff;
	std::stringstream ss;

protected:
	std::streamsize xsputn(const char* ptr, std::streamsize count) override
	{
		//std::cout << "xsputn: \"" << std::string(ptr, ptr + count) << "\"" << std::endl;
		return std::streambuf::xsputn(ptr, count);

		int free = this->epptr() - pptr();
		std::memcpy(pptr(), ptr, count);
		this->pbump(count);
		return count;
	}

	int_type overflow(int_type ch)
	{
		std::cout << "Overflow! m_buffer = \"" << std::string(m_buff.begin(), m_buff.end()) << "\"" << std::endl;
		//m_buff.clear();
		this->setp(m_buff.data(), m_buff.data() + m_buff.capacity());

		if (ch != std::char_traits<char>::eof())
		{
			char c = ch;
			this->xsputn(&c, 1);
			return std::char_traits<char>::not_eof(ch);
		}

		// giusto ?!?!? not_eof?
		//return std::char_traits<char>::eof() + 1;
		return std::char_traits<char>::not_eof(ch);
	}

	int sync(void) override
	{
		std::cout << "sync" << std::endl;
		this->overflow(std::char_traits<char>::eof());
		return 0;
	}
};
//std::stringstream

#include <gnuplotpp/gnuplotpp.hpp>

#include <random>

int safe_main_3(int argc, char** argv)
{
	std::cout << "Hello There!" << std::endl;

	using namespace lc;

	std::default_random_engine e;
	std::normal_distribution n;

	auto randVec = [&](size_t N, double mean = 0.0, double sigma = 1.0) -> std::vector<double>
	{
		std::vector<double> v; v.reserve(N);
		for (size_t i = 0; i < N; i++)
			v.push_back(n(e) * sigma + mean);
		return v;
	};

	{
		Gnuplotpp gp; gp.setTerminal(Gnuplotpp::Terminal::Qt);
		gp.addRdbuf(std::cout.rdbuf());
		gp.writeCommandsOnFile("a.p");

		gp << "plot sin(x)" << std::endl;
		auto p = gp.plot(randVec(100));
		gp.draw({ p });
	}

	return 0;

	MyStream streamBuf;
	std::ostream ostr(&streamBuf);
	lc::PipeStreamBuf pipe("gnuplot --persist");
	std::ostream gp(&pipe);

	gp << "plot sin(x)" << std::endl;
	gp << std::endl;
	gp << std::endl;
	gp << std::endl;
	gp << std::endl;
	gp << std::endl;
	gp << std::endl;
	gp << std::endl;

	return 0;

	ostr << "Hello There!" << std::endl;
	ostr << "Hello There2!" << std::endl;
	ostr << "Hello There3!" << std::endl;
	ostr << std::endl;
	ostr.flush();

	return 0;
}