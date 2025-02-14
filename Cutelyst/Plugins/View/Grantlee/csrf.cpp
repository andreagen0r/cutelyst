/*
 * SPDX-FileCopyrightText: (C) 2017-2022 Matthias Fehring <mf@huessenbergnetz.de>
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "csrf.h"

#include <Cutelyst/Context>
#include <Cutelyst/ParamsMultiMap>
#include <Cutelyst/Request>
#include <Cutelyst/Response>
#include <grantlee/exception.h>
#include <grantlee/parser.h>

#ifdef PLUGIN_CSRFPROTECTION_ENABLED
#    include <Cutelyst/Plugins/CSRFProtection/CSRFProtection>
#endif

Grantlee::Node *CSRFTag::getNode(const QString &tagContent, Grantlee::Parser *p) const
{
    Q_UNUSED(tagContent);
    return new CSRF(p);
}

CSRF::CSRF(Grantlee::Parser *parser)
    : Grantlee::Node(parser)
{
}

void CSRF::render(Grantlee::OutputStream *stream, Grantlee::Context *gc) const
{
#ifdef PLUGIN_CSRFPROTECTION_ENABLED
    // In case cutelyst context is not set as "c"
    auto c = gc->lookup(m_cutelystContext).value<Cutelyst::Context *>();
    if (!c) {
        const QVariantHash hash = gc->stackHash(0);
        auto it                 = hash.constBegin();
        while (it != hash.constEnd()) {
            if (it.value().userType() == qMetaTypeId<Cutelyst::Context *>()) {
                c = it.value().value<Cutelyst::Context *>();
                if (c) {
                    m_cutelystContext = it.key();
                    break;
                }
            }
            ++it;
        }

        if (!c) {
            return;
        }
    }

    *stream << Cutelyst::CSRFProtection::getTokenFormField(c);
#else
    Q_UNUSED(stream)
    Q_UNUSED(gc)
    qWarning("%s", "The CSRF protection plugin has not been built.");
#endif
}

#include "moc_csrf.cpp"
