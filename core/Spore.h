/*!
 * \file Spore.h
 * \date 2016/08/19 12:38
 *
 * \author kicsy
 * Contact: lmj07luck@126.com
 *
 * \brief 
 *
 * TODO: long description
 *
 * \note
*/
#pragma once
#include <shared_mutex>
#include <mutex>
#include <vector>
#include <unordered_map>
#include "Statement.h"
#include "Pin.h"

namespace fs
{
	class Spore : public std::enable_shared_from_this<Spore>
	{
	public:
		Spore(const std::string &name = std::string());
		Spore(const Spore&) = delete;
		Spore() = delete;
		~Spore();
		PW_Spore parent();
		std::string name();

		bool input(P_Pin pin, P_Data data);
		std::vector<P_Pin> pins();
		P_Pin getPin(const std::string &name);
		P_Pin addPin(const std::string &name, Pin_Process process = nullptr);

		std::vector<P_Spore> childs();
		P_Spore addChild(P_Spore child);

		static P_Spore newSpore(const std::string &name = "");

		static Pin_Process ENTRY;
	protected:
		void buildSession(IdType sessionId);
		void releaseSession(IdType sessionId);
		void process(Pin_Process pprocess, P_Data data);
		P_Path create_or_find_Path(P_Pin from, P_Pin to, const std::string &name = "");

	protected:
		std::string _name;
		P_Spore _parent;

		std::vector<P_Spore> _childs;
		std::shared_mutex _childs_mutex;


		std::unordered_map<std::string, P_Pin> _pins;
		std::shared_mutex _pins_mutex;

		std::vector<P_Path> _paths;
		std::shared_mutex _paths_mutex;


		std::unordered_map<IdType, P_AnyValues> _sessionValues;
		std::shared_mutex _session_mutex;
		friend class Session;
		friend class Path;
	};
}


