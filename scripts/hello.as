class object_data
{
    object_data()
    {
    }

    object_data(string className, int price)
    {
        _className = className;
        _price = price;
    }

    object_data(const object_data &in other)
    {
        this._className = other._className;
        this._price = other._price;
    }

    string get_class_name()
    {
        return _className;
    }

    int get_price()
    {
        return _price;
    }

    void print_data()
    {
        string info = _className += " "; 
        info += formatInt(_price);
        info += "\n";
        sqf::print(info);
    }
   
    private string _className;
    private int _price;
}


void main()
{
    bool hasDiscout = true;
    int price = 600;
    string infoText = "price: ";

    if(hasDiscout)
    {
        price = price / 2;
        infoText += formatInt(price);
    }
    //print(infoText);

    object_data value = object_data("A3_Cargo_Truck_F", price);

    value.print_data();

    json test;
    test.push_back(10);
    test.push_back(20);
    test.push_back(15);
    test.print();
    
}