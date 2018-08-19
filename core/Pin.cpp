#include <algorithm>
#include "Pin.h"
#include "Spore.h"
#include "Path.h"
using namespace fs;

fs::Pin::~Pin()
{
}

void fs::Pin::push(C_Data data)
{
	if (_type == Pin_Type::IN_PIN)
	{
		P_Spore spore = _parent.lock();
		if (spore)
		{
			spore->input(_this.lock(), data);
		}
	}
	else if (_type == Pin_Type::OUT_PIN)
	{
		for (auto &path : _paths)
		{
			if (path != nullptr && path->isvalid())
			{
				path->move(data);
			}
		}
	}
	for (auto & child : _childs)
	{
		child->push(data);
	}
}

std::tuple<bool, P_Path> fs::Pin::connect(std::shared_ptr<Pin> otherPin, const std::string &pathName)
{
	if (!otherPin)
	{
		return std::move(std::make_tuple(false, nullptr));
	}
	P_Spore thisSpore = parent();
	P_Spore otherSpore = otherPin->parent();
	if (!thisSpore || !otherSpore)
	{
		return std::move(std::make_tuple(false, nullptr));
	}
	if (thisSpore->parent() == otherSpore->parent())
	{
		//����Pin������SporeΪͬ����ͨ��Path����
		auto iter = std::find_if(_paths.begin(), _paths.end(), [&](P_Path& pp) {
			return pp->inPin().get() == this && pp->outPin().get() == otherPin.get() ||
				pp->outPin().get() == this && pp->inPin().get() == otherPin.get();
		});
		if (iter != _paths.end())
		{
			return std::move(std::make_tuple(false, *iter));
		}

		P_Path path;
		if ((_type == Pin_Type::IN_PIN) && (otherPin->_type == Pin_Type::OUT_PIN))
		{
			path = std::make_shared<fs::Path>(pathName, 0);
			path->_inPin = _this.lock();
			path->_outPin = otherPin;
		}
		else if ((_type == Pin_Type::OUT_PIN) && (otherPin->_type == Pin_Type::IN_PIN))
		{
			path = std::make_shared<fs::Path>(pathName, 0);
			path->_outPin = _this.lock();
			path->_inPin = otherPin;
		}
		path->_this = path;
		_paths.push_back(path);
		otherPin->_paths.push_back(path);
		return std::move(std::make_tuple(false, path));
	}

	P_Pin parentPin;
	P_Pin childPin;
	if (thisSpore == otherSpore->parent())
	{
		//����Pin��SporeΪ���ӹ�ϵ����Pin�ϲ�
		parentPin = _this.lock();
		childPin = otherPin;

	}
	else if (otherSpore == thisSpore->parent())
	{
		//����Pin��SporeΪ���ӹ�ϵ����Pin�ϲ�
		parentPin = otherPin;
		childPin = _this.lock();
	}
	else
	{
		return std::make_tuple(false, nullptr);
	}

	if (parentPin->type() != childPin->type())
	{
		return std::move(std::make_tuple(false, nullptr));
	}
	if (parentPin->type() == Pin_Type::IN_PIN)
	{
		parentPin->_childs.push_back(childPin);
	}
	else if (parentPin->type() == Pin_Type::OUT_PIN)
	{
		childPin->_childs.push_back(parentPin);
	}
	return std::move(std::make_tuple(true, nullptr));
}

fs::Pin_Type fs::Pin::type() const
{
	return _type;
}

std::vector<P_Path> fs::Pin::paths()
{
	return _paths;
}
