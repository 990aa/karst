using SkiaSharp;

namespace karst.Converters;

public static class BindingConverters
{
    public static readonly IValueConverter IsNotNull = new FuncConverter<object, bool>(
        value => value != null, 
        value => throw new NotImplementedException());

    public static readonly IValueConverter IsEqual = new FuncConverter<object, object, object, bool>(
        (value, parameter, culture) => Equals(value, parameter),
        (value, parameter, culture) => throw new NotImplementedException());

    public static readonly IValueConverter SKColorToColor = new FuncConverter<SKColor, Color>(
        skColor => Color.FromRgba(skColor.Red, skColor.Green, skColor.Blue, skColor.Alpha),
        color => throw new NotImplementedException());
}

public class FuncConverter<TFrom, TTo> : IValueConverter
{
    private readonly Func<TFrom, TTo> _convert;
    private readonly Func<TTo, TFrom> _convertBack;

    public FuncConverter(Func<TFrom, TTo> convert, Func<TTo, TFrom> convertBack = null)
    {
        _convert = convert;
        _convertBack = convertBack;
    }

    public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
    {
        return _convert((TFrom)value);
    }

    public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
    {
        return _convertBack != null ? _convertBack((TTo)value) : throw new NotImplementedException();
    }
}

public class FuncConverter<TFrom, TParam, TTo> : IValueConverter
{
    private readonly Func<TFrom, TParam, TTo> _convert;
    private readonly Func<TTo, TParam, TFrom> _convertBack;

    public FuncConverter(Func<TFrom, TParam, TTo> convert, Func<TTo, TParam, TFrom> convertBack = null)
    {
        _convert = convert;
        _convertBack = convertBack;
    }

    public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
    {
        return _convert((TFrom)value, (TParam)parameter);
    }

    public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
    {
        return _convertBack != null ? _convertBack((TTo)value, (TParam)parameter) : throw new NotImplementedException();
    }
}

public class FuncConverter<TFrom, TParam, TCulture, TTo> : IValueConverter
{
    private readonly Func<TFrom, TParam, TCulture, TTo> _convert;
    private readonly Func<TTo, TParam, TCulture, TFrom> _convertBack;

    public FuncConverter(Func<TFrom, TParam, TCulture, TTo> convert, Func<TTo, TParam, TCulture, TFrom> convertBack = null)
    {
        _convert = convert;
        _convertBack = convertBack;
    }

    public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
    {
        return _convert((TFrom)value, (TParam)parameter, (TCulture)(object)culture);
    }

    public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
    {
        return _convertBack != null ? _convertBack((TTo)value, (TParam)parameter, (TCulture)(object)culture) : throw new NotImplementedException();
    }
}