using System;

namespace GhostbaitModelCreator {

    internal static class Extensions {

        public static T SetFlag<T>(this Enum enumValue, T enumFlag, bool on = true) {
            Type underlyingType = Enum.GetUnderlyingType(enumValue.GetType());

            dynamic value = Convert.ChangeType(enumValue, underlyingType);
            dynamic flag = Convert.ChangeType(enumFlag, underlyingType);

            //value = on ? (value | flag) : (value & ~flag);

            if (on) {
                value |= flag;
            } else {
                value &= ~flag;
            }

            return (T)value;
        }
    }
}
