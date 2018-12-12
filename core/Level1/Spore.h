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
#include "Property.h"
#include "Schema.h"
#include "WarpPin.h"

namespace fs
{
	namespace L1
	{
		class Spore : public Property , public std::enable_shared_from_this<Spore>
		{
		public:
			Spore(const std::string &name, const PropertyInitList &&initList = PropertyInitList());
			Spore(const Spore&) = delete;
			Spore() = delete;
			~Spore();
			bool input(const P_Pin &pin, const P_Data &data);

			std::vector<P_Pin> pins();
			P_Pin getPin(const std::string &name);
			P_Pin addPin(P_Pin &pin);
			bool deletePin(P_Pin &pin);
			bool deletePin(const std::string &name);

			static P_Property newSpore(const std::string &name, const PropertyInitList &&initList = PropertyInitList());
		protected:
			virtual PW_Property  weakFromThis()
			{
				return std::static_pointer_cast<Property>(shared_from_this());
			}

			void buildSession(IdType sessionId);
			void releaseSession(IdType sessionId);
			void cleanAllSession();
			void process(const P_Pin &pin, const P_Data &data);
			P_Path create_or_find_Path(P_Pin from, P_Pin to, const std::string &name = "");
			bool deletePath(const P_Path &path);

		protected:
			std::unordered_map<std::string, P_Pin> _pins;
			std::shared_mutex _pins_mutex;

			std::vector<P_Path> _paths;
			std::shared_mutex _paths_mutex;

			std::unordered_map<IdType, P_AnyValues> _session_local_Values;
			std::shared_mutex _session_local_mutex;

			friend class Session;
			friend class Path;
		};
	}
}


