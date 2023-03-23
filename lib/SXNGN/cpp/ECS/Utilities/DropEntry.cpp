#include <ECS/Utilities/DropEntry.hpp>
#include <iostream>



	template<typename T>
	SXNGN::ECS::DropEntry<T>::DropEntry() {
		weight = 1;
		max_weight = 100;
		min_weight = 0;
		reoccurance_penalty = 100;
		accumulation = 10;
	}



	template<typename T>
	SXNGN::ECS::DropEntry<T>* SXNGN::ECS::DropEntry<T>::find_event_by_type(T search_value)
	{
		DropEntry* ret_val = nullptr;
		if (value == search_value)
		{
			return this;
		}
		for (int i = 0; i < this->children.size(); i++)
		{
			ret_val = this->children.at(i).find_event_by_type(search_value);
			if (ret_val != nullptr)
			{
				break;
			}
		}
		return ret_val;

	}

	template<typename T>
	SXNGN::ECS::DropEntry<T>* SXNGN::ECS::DropEntry<T>::generate_event(DropEntry<T>* node)
	{
		//If we are here, this node or one of its children will be chosen, so reduce the future probability of this branch.
		node->weight -= node->reoccurance_penalty;
		node->weight = std::clamp(node->weight, node->min_weight, node->max_weight);
		if (node->children.size() == 0)
		{
			//if this node has no children, this is the end of the function.
			return node;
		}
		else if (node->children.size() == 1)
		{
			//if there is only one child then use it
			return generate_event(&(node->children[0]));
		}
		//algorithm for choosing which event out of this node's children will occur
		//1. sum up total weight of children
		//2. generate random number between 0 and total weight
		//3. Iterate through all children again and accumulate their weight, return the child that makes accumulated weight > random number
		// result is that a child's chance to be chosen is proportional to their weight
		int total_weight = 0;
		for (int i = 0; i < node->children.size(); i++)
		{
			total_weight += node->children[i].weight;
		}
		if (total_weight < 1)
		{
			total_weight = 1;
		}
		int random_number = std::rand() % total_weight;
		int running_weight = 0;
		bool still_searching = true;
		int chosen_event = -1;
		for (int i = 0; i < node->children.size(); i++)
		{
			running_weight += node->children[i].weight;
			if (still_searching && running_weight > random_number)
			{
				chosen_event = i; //save chosen child for later 
				still_searching = false;

			}
			else
			{
				node->children[i].weight += node->children[i].accumulation; //make children that were not chosen more likely to be chosen in the future
				node->children[i].weight = std::clamp(node->children[i].weight, node->children[i].min_weight, node->children[i].max_weight);
			}
		}
		if (still_searching)
		{
			return node;
		}
		else
		{
			return generate_event(&(node->children[chosen_event]));
		}
	}

	template<typename T>
	void SXNGN::ECS::DropEntry<T>::print_event_table(const std::string& prefix, SXNGN::ECS::DropEntry<T> node, bool last_child)
	{
		if (node.weight != -1)
		{
			std::cout << prefix;
			std::cout << (last_child ? "L--" : "|--");
			std::cout << "[" << party_event_type_enum_to_string()[node.value] << "]{" << node.weight  << "/" << node.max_weight << "+" << node.accumulation << "}" << std::endl;

			for (int i = 0; i < node.children.size(); i++)
			{
				if (i == node.children.size() - 1)
				{
					print_event_table(prefix + (last_child ? "    " : "|   "), node.children[i], true);
				}
				else
				{
					print_event_table(prefix + (last_child ? "    " : "|   "), node.children[i], false);
				}
			}

		}
	}

	template<typename T>
	void SXNGN::ECS::DropEntry<T>::print_event_table(DropEntry<T> node)
	{
		std::cout << "Event Table Start" << std::endl;
		print_event_table("", node, false);
		std::cout << "Event Table End " << std::endl;
	}

	template<typename T>
	std::string SXNGN::ECS::DropEntry<T>::to_std_string()
	{
		return party_event_type_enum_to_string()[value];
	}

	template class SXNGN::ECS::DropEntry<SXNGN::ECS::PartyEventType>;
